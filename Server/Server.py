import builtins
import socket
import socketserver
import struct
import random
import sys
from typing import NamedTuple, Set, Tuple
from enum import IntEnum
from datetime import datetime

DATASIZE = 0x1000
CODES: Set[int] = set()
printmode = 1

SPSTRUCT = struct.Struct(f"B7sII{DATASIZE - 16}s")
EMPTY = b"\0\0\0\0\0\0\0\0"

CONNECT_MAGIC = 0x1F2F3F4F


class ClientHeaders(IntEnum):
    REQUEST_CODE = 0x00
    JOIN = 0x01

    DATA = 0x10
    DATA_VERIFIED = 0x11

    QUERY_VERIFICATION = 0x20

    LEAVE = 0xFF


class ServerHeaders(IntEnum):
    CODES = 0x00
    NEW_PLAYER = 0x01

    DATA = 0x10
    DATA_VERIFIED = 0x11

    RECEIVED = 0x20
    VERIFY_CLEAR = 0x21

    INVALID_HEADER = 0x80
    NO_ROOM = 0x81
    UNKNOWN_PLAYER = 0x82

    LEAVE = 0xFF


class ServerPacket(NamedTuple):
    header: int
    game: bytes
    player: int
    room: int
    data: bytes

    @classmethod
    def frombytes(cls, data):
        return cls._make(SPSTRUCT.unpack(data))

    def tobytes(self) -> bytes:
        s = ServerPacket(self.header, self.game,
                         self.player, self.room, self.data)
        return SPSTRUCT.pack(*tuple(s))

    def getint(self, index) -> int:
        return int.from_bytes(self.data[index * 4:(index + 1) * 4], 'little')


def hex(i: int) -> str:
    return builtins.hex(i).upper()[2:].zfill(8)


def randint() -> int:
    return random.randint(1, 0xFFFFFFFF)

def print(*args):
    builtins.print(datetime.now().strftime("[%m-%d-%y %H:%M:%S]"), *args)


class Player:
    def __init__(self, client, code, game: "Game") -> None:
        self.client = client
        self.code: int = code
        self.room: Room = game.get_room(0)
        self.game = game
        game.join(self)
        self.room.players.add(self)

    def move(self, room: "Room"):
        self.room.leave(self)
        self.room = room
        self.room.join(self)
        if self.room not in self.game.rooms:
            self.game.rooms.add(self.room)

    def __hash__(self) -> int:
        return self.code

    def deliver(self, packet: ServerPacket):
        if printmode == 2:
            print(f"[{self.game.name}/{hex(self.room.code)}] {ServerHeaders(packet.header).name} from {hex(packet.player)} to {hex(self.code)}")
        server.socket.sendto(packet.tobytes(), self.client)


class Room:
    def __init__(self, game: "Game", code=0) -> None:
        self.code = code
        self.players: Set[Player] = set()
        self.game: Game = game

        self.verifiying = False
        self.verified: Set[Player] = set()
        self.vcopy: Set[Player] = set()

    def __hash__(self) -> int:
        return self.code

    def join(self, player: Player):
        if player not in self.players:
            self.players.add(player)
            if printmode:
                print(
                    f"[{self.game.name}/{hex(self.code)}] Player {hex(player.code)} joined")

    def leave(self, player: Player):
        if player in self.players:
            self.players.remove(player)
            if printmode:
                print(
                    f"[{self.game.name}/{hex(self.code)}] Player {hex(player.code)} left")

    def deliver(self, packet: ServerPacket, sender: Player) -> int:
        sent = 0
        queueSends = False
        if packet.header in (ClientHeaders.DATA_VERIFIED, ClientHeaders.QUERY_VERIFICATION):
            if not self.vcopy:
                self.vcopy = self.players.copy()
            if sender not in self.vcopy:
                # impostor (we let them in bc we are Stupid Crewmates
                self.vcopy.add(sender)
            if self.players - self.vcopy:
                # some people left. L
                self.vcopy &= self.players

            if packet.header == ClientHeaders.DATA_VERIFIED:
                self.verifiying = True
                self.verified.add(sender)
                if printmode:
                    print(
                        f"[{self.game.name}/{hex(self.code)}] Verifying {len(self.verified)}/{len(self.vcopy)}")

            elif not self.verifiying:
                # hack. if querying and not verifying, use this to force "yes"
                self.verified = self.players

            if self.vcopy == self.verified:
                pl = b"".join([x.code.to_bytes(8, "little")
                               for x in self.verified])
                queueSends = True
                self.verified.clear()
                self.vcopy.clear()
                self.verifiying = False
            else:
                sender.deliver(ServerPacket(ServerHeaders.RECEIVED,
                                            self.game.bytename, sender.code, self.code, bytes()))

        if packet.header != ClientHeaders.QUERY_VERIFICATION:
            for player in self.players:
                if player.code != sender.code:
                    player.deliver(packet)
                    sent += 1

        if queueSends:
            for player in self.players:
                player.deliver(ServerPacket(ServerHeaders.VERIFY_CLEAR,
                                            self.game.bytename, player.code, self.code, pl))
        return sent


class EmptyRoom(Room):
    def __init__(self, game: "Game") -> None:
        super().__init__(game, code=0)

    def deliver(self):
        return

    def join(self, player: Player):
        self.players.add(player)

    def leave(self, player: Player):
        try:
            self.players.remove(player)
        except:
            pass


class Game:
    def __init__(self, name: bytes) -> None:
        self.name: str = name.decode().rstrip("\x00")
        self.bytename: bytes = name
        self.rooms: Set[Room] = {EmptyRoom(self)}

    def __hash__(self) -> int:
        return hash(self.name)

    def join(self, player: Player) -> None:
        self.rooms.add(player.room)
        if printmode:
            print(
                f"[{self.name}] New player {hex(player.code)}{f' in {hex(player.room.code)}' if player.room.code else ''}")

    def leave(self, player: Player) -> bool:
        for r in self.rooms.copy():
            if player in r.players:
                r.players.remove(player)
                self.rooms.remove(r)
                if printmode:
                    print(
                        f"[{self.name}] Player {hex(player.code)} in room {hex(player.room.code)} has left")
                return True
        return False

    def get_room(self, room: int) -> Room:
        for x in self.rooms:
            if x.code == room:
                return x
        return self.get_room(0)

    def deliver(self, packet: ServerPacket, sender: Player) -> int:
        return sender.room.deliver(packet, sender)


class Handler(socketserver.BaseRequestHandler):
    def handle(self):
        self.server: Server
        self.request: Tuple[bytes, socket.socket]

        data = ServerPacket.frombytes(self.request[0])
        try:
            ClientHeaders(data.header)
        except:
            return  # not a valid header
        if printmode == 2:
            print(
                f"(SERVER) {ClientHeaders(data.header).name} from {hex(data.player)} to server")
        if not data.player:
            if data.header != ClientHeaders.REQUEST_CODE and data.room != CONNECT_MAGIC:
                return  # shhhhhh
            player = Player(self.client_address, randint(),
                            self.server.get_game(data.game))

            return self.send(ServerHeaders.CODES, player)

        # it begins

        if data.header == ClientHeaders.REQUEST_CODE:
            # let's give a room code back and reassign

            p = self.server.resolve_player(data.player, data.game)
            if not p:
                return self.send_raw(ServerPacket(ServerHeaders.UNKNOWN_PLAYER, data.game, data.player, data.room, bytes()))
            if p.room.code:
                return self.send(ServerHeaders.CODES, p, len(p.room.players).to_bytes(4, 'little') + b''.join([x.code.to_bytes(8, "little")
                                                                                                               for x in p.room.players - {p}]))
            roomid = 0
            while (roomid in [x.code for x in p.game.rooms]):
                roomid = ((randint() & 0xFFFFFFFF) & ~
                          data.getint(0)) | data.getint(1)
            r = Room(p.game, roomid)
            p.move(r)
            return self.send(ServerHeaders.CODES, p, len(r.players).to_bytes(4, 'little') + b''.join([x.code.to_bytes(8, "little")
                                                                                                      for x in r.players - {p}]))
        if data.header == ClientHeaders.JOIN:
            # data will be how many players there are so the client can decide "this is too many players"
            p = self.server.resolve_player(data.player, data.game)
            r = p.game.get_room(data.room)
            if not r.code:
                return self.send(ServerHeaders.NO_ROOM, p)
            p.move(r)
            self.send(ServerHeaders.CODES, p, len(r.players).to_bytes(4, 'little') + b''.join([x.code.to_bytes(8, "little")
                                                                                               for x in r.players - {p}]))
            r.deliver(ServerPacket(ServerHeaders.NEW_PLAYER,
                                   data.game, p.code, r.code, bytes()), p)
        if data.header in (ClientHeaders.DATA, ClientHeaders.DATA_VERIFIED, ClientHeaders.QUERY_VERIFICATION):
            if not data.room:
                return self.send_raw(ServerPacket(ServerHeaders.INVALID_HEADER, data.game, data.player, data.room, bytes()))
            p = self.server.resolve_player(data.player, data.game, data.room)
            if not p:
                return self.send_raw(ServerPacket(ServerHeaders.UNKNOWN_PLAYER, data.game, data.player, data.room, bytes()))
            p.room.deliver(data, p)

        if data.header == ClientHeaders.LEAVE:
            p = self.server.resolve_player(data.player, data.game, data.room)
            if not p or not p.room:
                return
            p.room.leave(p)
            if not p.room.code:
                return
            if p.room.players:
                p.room.deliver(data, p)
            else:
                p.game.rooms.remove(p.room)
                if printmode:
                    print(f"[{p.game.name}] Room {hex(p.room.code)} disbanded")

    def send(self, header, player: Player, data=()):
        self.request[1].sendto(ServerPacket(header, player.game.bytename,
                                            player.code, player.room.code, bytes(data)).tobytes(), player.client)

    def send_raw(self, packet: ServerPacket):
        self.request[1].sendto(packet.tobytes(), self.client_address)


class Server(socketserver.ThreadingUDPServer):
    def __init__(self, server_address) -> None:
        super().__init__(server_address, Handler)
        self.games: Set[Game] = set()

    def get_game(self, name: bytes, create=True) -> Game:
        for game in self.games:
            if game.bytename == name:
                return game
        if not create:
            return False
        # game doesn't exist, let's make it
        g = Game(name)
        self.games.add(g)
        if printmode:
            print(f"Game {g.name} created")
        return g

    def resolve_player(self, player: int, game: bytes = None, room: int = -1):
        g = None
        r = None
        if game:
            g = self.get_game(game, False)
            if g:
                if room != -1:
                    for x in g.rooms:
                        if x.code == room:
                            r = x
                            break

        def searchR(player, r=r):
            for x in r.players:
                if x.code == player:
                    return x
            return None

        def searchG(player, g=g):
            for r in g.rooms:
                if ret := searchR(player, r):
                    return ret
            return None

        if not (g or r):

            def search(player):
                for g in self.games:
                    if ret := searchG(player, g):
                        return ret
                return None

        elif g and not r:
            search = searchG
        else:
            search = searchR
        return search(player)


if len(sys.argv) < 2:
    print("must have IP and port")
    exit()

ip = sys.argv[1]
p = int(sys.argv[2])

server: Server = Server((ip, p))

if len(sys.argv) > 3:
    for x in sys.argv[2:]:
        if x == "silent":
            printmode = 0
        if x == "debug":
            printmode = 1
        if x == "verbose":
            printmode = 2

print("starting..")
server.serve_forever()
