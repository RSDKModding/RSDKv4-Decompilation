#!/bin/sh

[ -f /app/share/sonic1/Data.rsdk ] && ln -sf /app/share/sonic1/Data.rsdk .
[ -f /app/share/sonic2/Data.rsdk ] && ln -sf /app/share/sonic2/Data.rsdk .

RSDKv4