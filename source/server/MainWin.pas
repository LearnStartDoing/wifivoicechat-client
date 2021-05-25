unit MainWin;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, IdBaseComponent, IdComponent, IdUDPBase, IdUDPServer,IdSocketHandle,_m_Tools,_PicTools,
  ExtCtrls, IdUDPClient, zlib,math,mmsystem,_dosbox;

type
  TMain = class(TForm)
    UDPS: TIdUDPServer;
    PrevImg: TImage;
    ProcTimer: TTimer;
    PreviewTimer: TTimer;
    StatusLst: TListBox;
    Cons_ProcIntervalSecTimer: TTimer;
    UserListBtn: TButton;
    SendTimer: TTimer;
    RestartTimer: TTimer;
    ClearBtn: TButton;
    LogSaveBtn: TButton;
    procedure UDPSStatus(ASender: TObject; const AStatus: TIdStatus;
      const AStatusText: String);
    procedure FormCreate(Sender: TObject);
    procedure UDPSUDPRead(Sender: TObject; AData: TStream; ABinding: TIdSocketHandle);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure ProcTimerTimer(Sender: TObject);
    procedure PreviewTimerTimer(Sender: TObject);
    procedure Cons_ProcIntervalSecTimerTimer(Sender: TObject);
    procedure UserListBtnClick(Sender: TObject);
    procedure SendTimerTimer(Sender: TObject);
    procedure RestartTimerTimer(Sender: TObject);
    procedure ClearBtnClick(Sender: TObject);
    procedure LogSaveBtnClick(Sender: TObject);
  private
    { Private 宣言 }
  public
    { Public 宣言 }
  end;

var
  Main: TMain;

implementation

{$R *.dfm}

uses _Cons,_PenPacket,_g721lib;

var
  CurrentBM:TBitmap;

procedure TMain.FormCreate(Sender: TObject);
var
  tc:TTIMECAPS;
begin
  SetPriorityLevel(4);

  timeGetDevCaps(@tc,sizeof(TTIMECAPS));
  timeBeginPeriod(tc.wPeriodMin);
//  DebugStr('Debug.マルチメディアタイマーの分解能 '+IntToStr(tc.wPeriodMin)+'~'+IntToStr(tc.wPeriodMax)+'/1000ms');

  StatusLst.Clear;

  CurrentBM:=TBitmap.Create;
  MakeBlankBM(CurrentBM,DSScrWidth,DSScrHeight,pf24bit);

  InitDSScr;

  Cons_Init;
  Cons_ProcIntervalSecTimer.Enabled:=True;

  SendTimer.Interval:=1;
  SendTimer.Enabled:=True;

  UDPS.Active:=True;
end;

procedure TMain.UDPSStatus(ASender: TObject; const AStatus: TIdStatus;
  const AStatusText: String);
begin
  StatusLst.Items.Add(AStatusText);
end;

procedure TMain.UDPSUDPRead(Sender: TObject; AData: TStream;
  ABinding: TIdSocketHandle);
var
  buf:array of byte;
  bufsize:integer;
  PeerIP:string;
  PeerPort:integer;
  Con:PCon;
  msg:string;
  idx:integer;
begin
  bufsize:=AData.Size;
  if bufsize=0 then exit;
  setlength(buf,bufsize);
  AData.ReadBuffer(buf[0],bufsize);

  msg:='';
//  msg:=format('%s:%d recv:%d ',[ABinding.PeerIP,ABinding.PeerPort,bufsize]);
{
  for idx:=0 to 32-1 do begin
    msg:=msg+inttohex(buf[idx],2);
  end;
}
  if msg<>'' then StatusLst.Items.Add(msg);

  PeerIP:=ABinding.PeerIP;
  PeerPort:=ABinding.PeerPort;
  if PeerIP='' then exit;
  if PeerPort=0 then exit;

  Con:=Cons_FindEqualCon(PeerIP,PeerPort);
  if Con=nil then begin
    Con:=Cons_FindNullCon;
    if Con=nil then exit;
    Cons_Init_PCon(Con,ConStatus_Login,PeerIP,PeerPort);
//    Cons_Init_PCon(Con,ConStatus_Execute,PeerIP,PeerPort);
  end;

  if Con=nil then exit;

  Cons_Buf_AddBuf(Con,buf,bufsize);

  ProcTimer.Enabled:=True;
end;

procedure TMain.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  LogSaveBtnClick(nil);
  UDPS.Active:=False;
end;

var
  gsPenPacket_SetStatus:TPenPacket_SetStatus;
  gsPenPacket_GetDSScr:TPenPacket_GetDSScr;
  gsPenPacket_SetDSScr:TPenPacket_SetDSScr;
  gsPenPacket_SetLatency:TPenPacket_SetLatency;
  gsPenPacket_Line:TPenPacket_Line;
  gsPenPacket_TickLine:TPenPacket_TickLine;
  gsPenPacket_DrawBox:TPenPacket_DrawBox;
  gsPenPacket_FillBox:TPenPacket_FillBox;
  gsPenPacket_Erase:TPenPacket_Erase;
  gsPenPacket_Sendg721:TPenPacket_Sendg721;
  gsPenPacket_ChangeRoom:TPenPacket_ChangeRoom;

procedure Con_Proc(Con:PCon);
var
  datapos:integer;
  PacketType:EPenPacketType;
  PacketSize:integer;
begin
  Cons_Buf_SetPos(Con,0);

  while(True) do begin
    datapos:=Cons_Buf_GetDataPos(Con);
    if datapos=-1 then exit;

    Cons_Buf_SetPos(Con,datapos);
    if Cons_Buf_GetRestSize(Con)<8 then exit;

    PacketType:=EPenPacketType(Cons_Buf_GetBuf32(Con));
    PacketSize:=Cons_Buf_GetBuf32(Con);

    if Cons_Buf_GetRestSize(Con)<PacketSize then exit;

    case PacketType of
      PenPacketType_SetStatus: begin
        if PacketSize=TPenPacket_SetStatus_Size then begin
          Cons_Buf_GetBuf(Con,addr(gsPenPacket_SetStatus),PacketSize);
          PenPacket_SetStatus_Proc(gsPenPacket_SetStatus,Con);
          PenPacket_UserList_GlobalSend;
        end;
      end;
      PenPacketType_GetDSScr: begin
        if PacketSize=TPenPacket_GetDSScr_Size then begin
          Cons_Buf_GetBuf(Con,addr(gsPenPacket_GetDSScr),PacketSize);
//          Main.StatusLst.Items.Add(format('%d',[gsPenPacket_GetDSScr.y]));
          if Con.Status<>ConStatus_Null then begin
            PenPacket_GetDSScr_Proc(gsPenPacket_GetDSScr,Con);
          end;
        end;
      end;
      PenPacketType_SetDSScr: begin
        if PacketSize=TPenPacket_SetDSScr_Size then begin
          Cons_Buf_GetBuf(Con,addr(gsPenPacket_SetDSScr),PacketSize);
//          PenPacket_SetDSScr_Proc(gsPenPacket_SetDSScr,Con);
        end;
      end;
      PenPacketType_SetLatency: begin
        if PacketSize=TPenPacket_SetLatency_Size then begin
          Cons_Buf_GetBuf(Con,addr(gsPenPacket_SetLatency),PacketSize);
          PenPacket_SetLatency_Proc(gsPenPacket_SetLatency,Con);
        end;
      end;
      PenPacketType_Line: begin
        if PacketSize=TPenPacket_Line_Size then begin
          Cons_Buf_GetBuf(Con,addr(gsPenPacket_Line),PacketSize);
          if Con.Status=ConStatus_Execute then begin
            PenPacket_Line_Proc(gsPenPacket_Line);
            PenPacket_Line_GlobalSend(gsPenPacket_Line);
          end;
        end;
      end;
      PenPacketType_TickLine: begin
        if PacketSize=TPenPacket_TickLine_Size then begin
          Cons_Buf_GetBuf(Con,addr(gsPenPacket_TickLine),PacketSize);
          if Con.Status=ConStatus_Execute then begin
            PenPacket_TickLine_Proc(gsPenPacket_TickLine);
            PenPacket_TickLine_GlobalSend(gsPenPacket_TickLine);
          end;
        end;
      end;
      PenPacketType_DrawBox: begin
        if PacketSize=TPenPacket_DrawBox_Size then begin
          Cons_Buf_GetBuf(Con,addr(gsPenPacket_DrawBox),PacketSize);
          if Con.Status=ConStatus_Execute then begin
            PenPacket_DrawBox_Proc(gsPenPacket_DrawBox);
            PenPacket_DrawBox_GlobalSend(gsPenPacket_DrawBox);
          end;
        end;
      end;
      PenPacketType_FillBox: begin
        if PacketSize=TPenPacket_FillBox_Size then begin
          Cons_Buf_GetBuf(Con,addr(gsPenPacket_FillBox),PacketSize);
          if Con.Status=ConStatus_Execute then begin
            PenPacket_FillBox_Proc(gsPenPacket_FillBox);
            PenPacket_FillBox_GlobalSend(gsPenPacket_FillBox);
          end;
        end;
      end;
      PenPacketType_Erase: begin
        if PacketSize=TPenPacket_Erase_Size then begin
          Cons_Buf_GetBuf(Con,addr(gsPenPacket_Erase),PacketSize);
          if Con.Status=ConStatus_Execute then begin
            PenPacket_Erase_Proc(gsPenPacket_Erase);
            PenPacket_Erase_GlobalSend(gsPenPacket_Erase);
          end;
        end;
      end;
      PenPacketType_Sendg721: begin
        if PacketSize=TPenPacket_Sendg721_Size then begin
          Cons_Buf_GetBuf(Con,addr(gsPenPacket_Sendg721),PacketSize);
          if Con.Status=ConStatus_Execute then begin
            PenPacket_Sendg721_Proc(gsPenPacket_Sendg721,Con);
          end;
        end;
      end;
      PenPacketType_ChangeRoom: begin
        if PacketSize=TPenPacket_ChangeRoom_Size then begin
          Cons_Buf_GetBuf(Con,addr(gsPenPacket_ChangeRoom),PacketSize);
          if Con.Status=ConStatus_Execute then begin
            PenPacket_ChangeRoom_Proc(gsPenPacket_ChangeRoom,Con);
          end;
        end;
      end;
    end;

    Cons_Buf_DelBuf(Con,Cons_Buf_GetPos(Con));
  end;
end;

procedure TMain.ProcTimerTimer(Sender: TObject);
var
  idx:integer;
  Con:PCon;
begin
  ProcTimer.Enabled:=False;

  for idx:=0 to ConsMax-1 do begin
    Con:=@Cons[idx];
    if Con.Status<>ConStatus_Null then Con_Proc(Con);
  end;

end;

procedure TMain.PreviewTimerTimer(Sender: TObject);
var
  y,x:integer;
  spw:PWordArray;
  dpb:PByteArray;
  col:word;
begin
  PreviewTimer.Enabled:=False;
  if assigned(CurrentBM)=False then exit;

  for y:=0 to DSScrHeight-1 do begin
    spw:=addr(DSScrBuf[y][0]);
    dpb:=CurrentBM.ScanLine[y];
    for x:=0 to DSScrWidth-1 do begin
      col:=spw[x];
      dpb[x*3+0]:=((col shr 10) and $1f) shl 3;
      dpb[x*3+1]:=((col shr 5) and $1f) shl 3;
      dpb[x*3+2]:=((col shr 0) and $1f) shl 3;
    end;
  end;

  BitBlt(PrevImg.Canvas.Handle,0,0,DSScrWidth,DSScrHeight,CurrentBM.Canvas.Handle,0,0,SRCCOPY);
  PrevImg.Refresh;
end;

procedure TMain.Cons_ProcIntervalSecTimerTimer(Sender: TObject);
begin
  Cons_ProcIntervalSec;
end;

procedure TMain.UserListBtnClick(Sender: TObject);
var
  idx:integer;
  msg:string;
  stridx:integer;
  macidx:integer;
  macaddr:uint64;
begin
  PreviewTimerTimer(nil);

  Main.StatusLst.Clear;
  for idx:=0 to ConsMax-1 do begin
    msg:='';
    with Cons[idx] do begin
      if Status<>ConStatus_Null then begin
        msg:=format('%d l%d lng%d r%d %d,%dsec %s:%d ',[idx,Latency,integer(UserLng),RoomNum,UserOnTime,TimeoutSec,IP,Port]);
        macaddr:=UserMacAddr;
        for macidx:=0 to 6-1 do begin
          msg:=msg+inttohex(macaddr and $ff,2);
          macaddr:=macaddr shr 8;
          if macidx<>5 then msg:=msg+':';
        end;
        msg:=msg+' ';
        for stridx:=0 to UserNameSize-1 do begin
          if UserName[stridx]=widechar(0) then break;
          msg:=msg+UserName[stridx];
        end;
        msg:=msg+' ';
        for stridx:=0 to UserMsgSize-1 do begin
          if UserMsg[stridx]=widechar(0) then break;
          msg:=msg+UserMsg[stridx];
        end;
      end;
    end;
    if msg<>'' then Main.StatusLst.Items.Add(msg);
  end;
end;

var
  lt:dword;
  intervaltime:double=0;
  curtime:double;

var
  decbuf:array[0..ConsMax-1,0..g721_BlockSamples-1] of smallint;

var
  desynccnt:integer=0;

procedure TMain.SendTimerTimer(Sender: TObject);
var
  ct:dword;
  decapp:array[0..ConsMax-1] of boolean;
  idx,useridx:integer;
  psrcbuf:psmallint;
  tmp32buf:array[0..g721_BlockSamples-1] of integer;
  tmp16buf:array[0..g721_BlockSamples-1] of smallint;
  tmpRoomNum:word;
  smpidx:integer;
  smp:integer;
  apply:boolean;
begin
  if intervaltime=0 then begin
    lt:=gettickcount;
    intervaltime:=g721_BlockSamples/g721_PCMFreq;
    curtime:=0;
  end;

  ct:=gettickcount;
  curtime:=curtime+((ct-lt)/1000);
  lt:=ct;

  if curtime<intervaltime then exit;
  curtime:=curtime-intervaltime;

  if intervaltime<=curtime then begin
    inc(desynccnt);
    caption:=format('desync=%d',[desynccnt]);
  end;

  for idx:=0 to ConsMax-1 do begin
    decapp[idx]:=False;
  end;

  for idx:=0 to ConsMax-1 do begin
    with Cons[idx] do begin
      if Cons[idx].Status=ConStatus_Execute then begin
        if g721_recvdelay<>0 then begin
          dec(g721_recvdelay);
          end else begin
          if g721_recv_readpos=g721_recv_writepos then begin
            g721_recvdelay:=Latency;
            end else begin
            decapp[idx]:=True;
            g721_blockdecode(@g721_state_dec,@g721_recvbuf[g721_recv_readpos],@decbuf[idx,0]);
            inc(g721_recv_readpos,g721_BlockBytes);
            if g721_recv_readpos=g721_recvbufsize then g721_recv_readpos:=0;
          end;
        end;
      end;
    end;
  end;

  for idx:=0 to ConsMax-1 do begin
    if Cons[idx].Status=ConStatus_Execute then begin
      apply:=False;
      FillMemory(addr(tmp32buf[0]),g721_BlockSamples*4,0);
      tmpRoomNum:=Cons[idx].RoomNum;
      for useridx:=0 to ConsMax-1 do begin
        if decapp[useridx]=True then begin
          if idx<>useridx then begin
            with Cons[useridx] do begin
              if Status=ConStatus_Execute then begin
                if tmpRoomNum=RoomNum then begin
                  apply:=True;
                  psrcbuf:=@decbuf[useridx,0];
                  for smpidx:=0 to g721_BlockSamples-1 do begin
                    smp:=((tmp32buf[smpidx]*7)+integer(psrcbuf^)) div 8;
//                    smp:=tmp32buf[smpidx]+integer(psrcbuf^);
                    inc(psrcbuf);
                    tmp32buf[smpidx]:=smp;
                  end;
                end;
              end;
            end;
          end;
        end;
      end;

      for smpidx:=0 to g721_BlockSamples-1 do begin
        smp:=tmp32buf[smpidx]*4;
        smp:=min(max(smp,-$2000),$1fff);
//        smp:=(smpidx*$100*(b+1)) and $fff;
        tmp16buf[smpidx]:=smallint(smp);
      end;

      if apply=True then begin
        try
          PenPacket_Sendg721_Send(@Cons[idx],@tmp16buf[0]);
          except else begin
          end;
        end;
      end;
    end;
  end;

  Cons_SendBuf_SendAll;
  
  exit;
  for smpidx:=0 to g721_BlockSamples-1 do begin
    smp:=tmp16buf[smpidx] div 32;
    Main.PrevImg.Canvas.Pixels[smpidx,smp]:=$ffffff;
  end;
end;

var
  RestartTimeout:integer=7*60*60; // restart server 7hour.

procedure TMain.RestartTimerTimer(Sender: TObject);
begin
  dec(RestartTimeout);
  if RestartTimeout<5 then begin // last 5sec.
    UDPS.Active:=False;
    ProcTimer.Enabled:=False;
    PreviewTimer.Enabled:=False;
    Cons_ProcIntervalSecTimer.Enabled:=False;
    SendTimer.Enabled:=False;
    StatusLst.Items.Add(format('timeout last=%dsec',[RestartTimeout]));
  end;
  if RestartTimeout=0 then begin
    RestartTimer.Enabled:=False;
    CreateDOSBOX_Direct(ExtractFilePath(Application.ExeName),ExtractFilename(Application.ExeName),'');
    Main.Close;
  end;
end;

procedure TMain.ClearBtnClick(Sender: TObject);
var
  packet:TPenPacket_FillBox;
begin
  packet.Color:=0;
  packet.lastx:=1;
  packet.lasty:=1;
  packet.curx:=DSScrWidth-1;
  packet.cury:=DSScrHeight-1;

  PenPacket_FillBox_GlobalSend(packet);

  ProcTimer.Enabled:=True;
end;

procedure TMain.LogSaveBtnClick(Sender: TObject);
begin
  LogStrLst.SaveToFile(LogFilename);
end;

end.

