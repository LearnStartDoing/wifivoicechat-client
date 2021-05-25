program voicechatserver;

uses
  Forms,
  MainWin in 'MainWin.pas' {Main};

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TMain, Main);
  Application.Run;
end.
