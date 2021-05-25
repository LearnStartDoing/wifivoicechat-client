unit _g721lib;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, ExtCtrls, StdCtrls, IdUDPBase;

{$WARN UNIT_PLATFORM OFF}
{$WARN SYMBOL_PLATFORM OFF}

const g721_PCMFreq=16384;
const g721_BlockBytes=1300;
const g721_BlockSamples=g721_BlockBytes*2;

type g721_state=array[0..128-1] of byte;
type pg721_state=^g721_state;

procedure g721_init(g721:pg721_state);
procedure g721_free(g721:pg721_state);
procedure g721_blockencode(g721:pg721_state;g721_signal:PSmallint;g721_byte:PByte);
function g721_blockdecode(g721:pg721_state;g721_byte:PByte;g721_signal:PSmallint):boolean;

implementation

{$L 'g721lib\g721lib.obj'}

function _g72x_init_state(g721:pg721_state):pg721_state; cdecl; external;
procedure _g721_blkencoder(g721_signal:PSmallint;g721_byte:PByte;count:integer;g721:pg721_state); cdecl; external;
procedure _g721_blkdecoder(g721_byte:PByte;g721_signal:PSmallint;count:integer;g721:pg721_state); cdecl; external;

procedure g721_init(g721:pg721_state);
begin
//  _g72x_init_state(g721);
end;

procedure g721_free(g721:pg721_state);
begin
end;

procedure g721_blockencode(g721:pg721_state;g721_signal:PSmallint;g721_byte:PByte);
begin
  _g721_blkencoder(g721_signal,g721_byte,g721_BlockSamples,g721);
end;

function g721_blockdecode(g721:pg721_state;g721_byte:PByte;g721_signal:PSmallint):boolean;
begin                        
  _g721_blkdecoder(g721_byte,g721_signal,g721_BlockSamples,g721);
  Result:=True;
end;

end.                       
