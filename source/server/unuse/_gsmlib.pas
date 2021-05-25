unit _gsmlib;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, ExtCtrls, StdCtrls, IdUDPBase;

{$WARN UNIT_PLATFORM OFF}
{$WARN SYMBOL_PLATFORM OFF}

const GSM_BlockCount=16;
const GSM_BlockSamples=320*GSM_BlockCount;
const GSM_BlockBytes=65*GSM_BlockCount;

type gsm_state=array[0..1024-1] of byte;
type pgsm_state=^gsm_state;

procedure gsm_init(gsm:pgsm_state);
procedure gsm_free(gsm:pgsm_state);
procedure gsm_blockencode(gsm:pgsm_state;gsm_signal:PSmallint;gsm_byte:PByte);
function gsm_blockdecode(gsm:pgsm_state;gsm_byte:PByte;gsm_signal:PSmallint):boolean;

implementation

{$L 'gsmlib\gsmlib.obj'}

function _gsm_create(gsm:pgsm_state):pgsm_state; cdecl; external;
procedure _gsm_destroy(gsm:pgsm_state); cdecl; external;
procedure _gsm_encode(gsm:pgsm_state;gsm_signal:PSmallint;gsm_byte:PByte); cdecl; external;
function _gsm_decode(gsm:pgsm_state;gsm_byte:PByte;gsm_signal:PSmallint):integer; cdecl; external;

procedure gsm_init(gsm:pgsm_state);
begin
  _gsm_create(gsm);
end;

procedure gsm_free(gsm:pgsm_state);
begin
  _gsm_destroy(gsm);
end;

procedure gsm_blockencode(gsm:pgsm_state;gsm_signal:PSmallint;gsm_byte:PByte);
var
  idx:integer;
begin
  for idx:=0 to GSM_BlockCount-1 do begin
    _gsm_encode(gsm,gsm_signal,gsm_byte);
    inc(gsm_byte,((GSM_BlockBytes div GSM_BlockCount) + 1) div 2);
    inc(gsm_signal,(GSM_BlockSamples div GSM_BlockCount) div 2);
    _gsm_encode(gsm,gsm_signal,gsm_byte);
    inc(gsm_byte,((GSM_BlockBytes div GSM_BlockCount) + 0) div 2);
    inc(gsm_signal,(GSM_BlockSamples div GSM_BlockCount) div 2);
  end;
end;

function gsm_blockdecode(gsm:pgsm_state;gsm_byte:PByte;gsm_signal:PSmallint):boolean;
var
  idx:integer;
begin
  Result:=False;

  for idx:=0 to GSM_BlockCount-1 do begin
    if _gsm_decode(gsm,gsm_byte,gsm_signal)=-1 then exit;
    inc(gsm_byte,((GSM_BlockBytes div GSM_BlockCount) + 1) div 2);
    inc(gsm_signal,(GSM_BlockSamples div GSM_BlockCount) div 2);
    if _gsm_decode(gsm,gsm_byte,gsm_signal)=-1 then exit;
    inc(gsm_byte,((GSM_BlockBytes div GSM_BlockCount) + 0) div 2);
    inc(gsm_signal,(GSM_BlockSamples div GSM_BlockCount) div 2);
  end;

  Result:=True;
end;

end.
