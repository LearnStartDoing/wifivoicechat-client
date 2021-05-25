unit MainWin;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, ExtCtrls, StdCtrls, Buttons, _SplitML, ComCtrls,
  IdBaseComponent, IdComponent, IdRawBase, IdRawClient, IdIcmpClient;

type
  TMain = class(TForm)
    ApplyBtn: TBitBtn;
    CancelBtn: TBitBtn;
    StartupTimer: TTimer;
    ServerNameLbl: TLabel;
    ServerNameEdt: TEdit;
    ServerPortLbl: TLabel;
    ServerPortEdt: TEdit;
    LatencyLbl: TLabel;
    LatencyLst: TComboBox;
    AutoDetectBtn: TButton;
    IdIcmpClient1: TIdIcmpClient;
    PingPrgBar: TProgressBar;
    NDSROMOpenDlg: TOpenDialog;
    procedure FormCreate(Sender: TObject);
    procedure StartupTimerTimer(Sender: TObject);
    procedure ApplyBtnClick(Sender: TObject);
    procedure CancelBtnClick(Sender: TObject);
    procedure AutoDetectBtnClick(Sender: TObject);
  private
    { Private 宣言 }
  public
    { Public 宣言 }
  end;

var
  Main: TMain;

implementation

uses SelMLWin, SetuppedWin;

{$R *.dfm}

const CRLF:string=char($0d)+char($0a);

var
  StartPath:string;

var
  NDSROMFilename:string;

type
  TNetworkSetting=packed record
    ID:array[0..16-1] of char;
    ServerName:array[0..128-1] of char;
    ServerPort:dword;
    Latency:dword;
    dummy00,dummy01:dword;
    dummy10,dummy11,dummy12,dummy13:dword;
  end;

var
  NetworkSetting:TNetworkSetting;

function NetworkSetting_FindID:integer;
var
  rfs:TFileStream;
  buf:array of char;
  bufsize:integer;
  findstr:string;
  idx,findidx:integer;
  finded:boolean;
begin
  rfs:=TFileStream.Create(NDSROMFilename,fmOpenRead);
  bufsize:=rfs.Size;
  setlength(buf,bufsize);
  rfs.ReadBuffer(buf[0],bufsize);
  rfs.Free;

  findstr:='NetworkSetting';
  for idx:=0 to bufsize-length(findstr)-1 do begin
    finded:=True;
    for findidx:=0 to length(findstr)-1 do begin
      if buf[idx+findidx]<>findstr[1+findidx] then finded:=False;
    end;
    if finded=True then begin
      Result:=idx;
      exit;
    end;
  end;

  Result:=-1;
end;

procedure NetworkSetting_LoadData;
var
  rfs:TFileStream;
  ofs:integer;
begin
  ofs:=NetworkSetting_FindID;
  if ofs=-1 then begin
    ShowMessage(GetMLStr('Fatal error! can not found NetworkSettingID.|致命的なエラー! NetworkSettingIDが見つかりませんでした。'));
    Application.Terminate;
    exit;
  end;

  rfs:=TFileStream.Create(NDSROMFilename,fmOpenRead);
  rfs.Position:=ofs;
  rfs.ReadBuffer(NetworkSetting,sizeof(TNetworkSetting));
  rfs.Free;
end;

procedure NetworkSetting_SaveData;
var
  wfs:TFileStream;
  ofs:integer;
begin
  ofs:=NetworkSetting_FindID;
  if ofs=-1 then begin
    ShowMessage(GetMLStr('Fatal error! can not found NetworkSettingID.|致命的なエラー! NetworkSettingIDが見つかりませんでした。'));
    Application.Terminate;
    exit;
  end;

  wfs:=TFileStream.Create(NDSROMFilename,fmOpenReadWrite);
  wfs.Position:=ofs;
  wfs.WriteBuffer(NetworkSetting,sizeof(TNetworkSetting));
  wfs.Free;
end;

procedure TMain.FormCreate(Sender: TObject);
begin
  Application.Title:='VoiceChatClient Network setting tool|VoiceChatClient ネットワーク設定ツール';
  Main.Caption:=Application.Title;

  StartPath:=ExtractFilePath(Application.ExeName);

{
  NDSROMFilename:='D:\MyDocuments\NDS\wifi_voicechat\_BOOT_MP.nds';
  NetworkSetting_LoadData;
}

  StartupTimer.Enabled:=True;
end;

const g721_PCMFreq=16384;
const g721_BlockBytes=1300;
const g721_BlockSamples=g721_BlockBytes*2;
const g721_BlockTime=g721_BlockSamples/g721_PCMFreq;

procedure TMain.StartupTimerTimer(Sender: TObject);
var
  idx:integer;
  msg:string;
begin
  StartupTimer.Enabled:=False;

  ServerNameEdt.Text:='';
  ServerPortEdt.Text:='';

  if GetMLTypeLoaded=False then begin
    if SelML.ShowModal=mrCancel then begin
      Application.Terminate;
      exit;
    end;
  end;

  Application.Title:=GetMLStr(Application.Title);
  Main.Caption:=Application.Title;

  SetMLLbl(ServerNameLbl);
  SetMLLbl(ServerPortLbl);
  SetMLLbl(LatencyLbl);
  SetMLBtn(AutoDetectBtn);
  SetMLBitBtn(ApplyBtn);
  SetMLBitBtn(CancelBtn);

  NDSROMOpenDlg.Title:=GetMLStr('Choose the NDSROM file that changes the setting.|設定を変更するNDSROMファイルを選択してください。');
  if NDSROMOpenDlg.Execute=False then begin
    Application.Terminate;
    exit;
  end;
  NDSROMFilename:=NDSROMOpenDlg.FileName;
  NetworkSetting_LoadData;

  ServerNameEdt.Text:=NetworkSetting.ServerName;
  ServerPortEdt.Text:=inttostr(NetworkSetting.ServerPort);

  LatencyLst.Clear;
  for idx:=0 to 4-1 do begin
    msg:=GetMLStr('%d. under %dmsec|%d. %dミリ秒以下');
    msg:=format(msg,[idx+1,trunc(((idx+1)/2)*g721_BlockTime*1000)]);
    LatencyLst.Items.Add(msg);
  end;
  LatencyLst.ItemIndex:=1;
  try
    LatencyLst.ItemIndex:=NetworkSetting.Latency-1;
    except else begin
    end;
  end;

end;

procedure TMain.ApplyBtnClick(Sender: TObject);
var
  msg:string;
  idx:integer;
begin
  with NetworkSetting do begin
    if 127<=length(ServerPortEdt.Text) then begin
      ShowMessage(GetMLStr('Port server name.|サーバ名が異常です。'));
      exit;
    end;
    for idx:=0 to 128-1 do begin
      ServerName[idx]:=char(0);
    end;
    for idx:=0 to length(ServerNameEdt.Text)-1 do begin
      ServerName[idx]:=ServerNameEdt.Text[1+idx];
    end;
    ServerPort:=strtointdef(ServerPortEdt.Text,0);
    Latency:=LatencyLst.ItemIndex+1;
    if ServerName='' then begin
      ShowMessage(GetMLStr('Port server name.|サーバ名が異常です。'));
      exit;
    end;
    if ServerPort=0 then begin
      ShowMessage(GetMLStr('Port number error.|ポート番号が異常です。'));
      exit;
    end;
  end;

  NetworkSetting_SaveData;
  msg:=GetMLStr('The change was preserved.'+CRLF+'%s|変更を保存しました。'+CRLF+'%s');
  ShowMessage(format(msg,[NDSROMFilename]));

  Application.Terminate;
end;

procedure TMain.CancelBtnClick(Sender: TObject);
begin
  Application.Terminate;
end;

procedure TMain.AutoDetectBtnClick(Sender: TObject);
var
  errmsg:string;
  idx:integer;
  t:dword;
  err:boolean;
  Latency:dword;
  cnt:integer;
  retry:integer;
  msg:string;
  dupcnt:integer;
begin
  errmsg:=GetMLStr('Ping error! Please confirm the server setting.|Ping error! サーバ設定を確認してください。');

  IdIcmpClient1.Host:=ServerNameEdt.Text;
  IdIcmpClient1.Port:=strtointdef(ServerPortEdt.Text,0);

  if (IdIcmpClient1.Host='') or (IdIcmpClient1.Port=0) then begin
    ShowMessage(errmsg);
    exit;
  end;

  cnt:=4;

  PingPrgBar.Position:=0;
  PingPrgBar.Min:=-2;
  PingPrgBar.Max:=cnt;

  Latency:=0;
  retry:=16;

  for idx:=-2 to cnt-1 do begin
    PingPrgBar.Position:=idx;
    PingPrgBar.Refresh;
    Application.ProcessMessages;
    err:=False;
    t:=0;
    while(True) do begin
      err:=False;
      t:=gettickcount;
      try
        for dupcnt:=0 to 4-1 do begin
          IdIcmpClient1.Ping();
        end;
        except else begin
          err:=True;
        end;
      end;
      if err=False then break;
      dec(retry);
      if retry=0 then break;
    end;
    if (err=True) or (IdIcmpClient1.ReplyStatus.ReplyStatusType<>rsEcho) then begin
      PingPrgBar.Position:=0;
      PingPrgBar.Min:=0;
      PingPrgBar.Max:=1;
      ShowMessage(errmsg);
      exit;
    end;
    t:=gettickcount-t;
    if 0<=idx then inc(Latency,t);
  end;

  PingPrgBar.Position:=0;
  PingPrgBar.Min:=0;
  PingPrgBar.Max:=1;

  Latency:=(Latency div 4) div dword(cnt);

  idx:=trunc((Latency/1000)/(g721_BlockTime/2));
  if LatencyLst.Items.Count<=idx then idx:=LatencyLst.Items.Count-1;
  LatencyLst.ItemIndex:=idx;

  msg:=GetMLStr('Network latency is %dmsec. set to %d.|ネットワーク遅延時間は%dミリ秒です。%dに設定しました。');
  ShowMessage(format(msg,[Latency,idx+1]));
end;

end.
