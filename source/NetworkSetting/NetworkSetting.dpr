program NetworkSetting;

uses
  Forms,
  MainWin in 'MainWin.pas' {Main},
  SelMLWin in 'SelMLWin.pas' {SelML},
  SetuppedWin in 'SetuppedWin.pas' {Setupped};

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TMain, Main);
  Application.CreateForm(TSelML, SelML);
  Application.CreateForm(TSetupped, Setupped);
  Application.Run;
end.
