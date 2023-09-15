object fmMain: TfmMain
  Left = 448
  Top = 209
  BorderStyle = bsSingle
  Caption = 'GATT Serial Client'
  ClientHeight = 449
  ClientWidth = 675
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object laValue: TLabel
    Left = 168
    Top = 16
    Width = 164
    Height = 13
    Caption = 'Hexadecimal value (32 bytes max):'
  end
  object btConnect: TButton
    Left = 8
    Top = 8
    Width = 75
    Height = 25
    Caption = 'Connect'
    TabOrder = 0
    OnClick = btConnectClick
  end
  object btDisconnect: TButton
    Left = 88
    Top = 8
    Width = 75
    Height = 25
    Caption = 'Disconnect'
    Enabled = False
    TabOrder = 1
    OnClick = btDisconnectClick
  end
  object edValue: TEdit
    Left = 336
    Top = 8
    Width = 249
    Height = 21
    TabOrder = 2
    Text = '11223344'
    OnKeyPress = edValueKeyPress
  end
  object btSend: TButton
    Left = 592
    Top = 8
    Width = 75
    Height = 25
    Caption = 'Send'
    Enabled = False
    TabOrder = 3
    OnClick = btSendClick
  end
  object lbLog: TListBox
    Left = 8
    Top = 48
    Width = 657
    Height = 361
    ItemHeight = 13
    TabOrder = 4
  end
  object btClear: TButton
    Left = 584
    Top = 416
    Width = 75
    Height = 25
    Caption = 'Clear'
    TabOrder = 5
    OnClick = btClearClick
  end
  object wclBluetoothManager: TwclBluetoothManager
    AfterOpen = wclBluetoothManagerAfterOpen
    OnClosed = wclBluetoothManagerClosed
    Left = 232
    Top = 152
  end
  object wclGattClient: TwclGattClient
    OnCharacteristicChanged = wclGattClientCharacteristicChanged
    OnConnect = wclGattClientConnect
    OnDisconnect = wclGattClientDisconnect
    OnMaxPduSizeChanged = wclGattClientMaxPduSizeChanged
    Left = 248
    Top = 248
  end
  object wclBluetoothLeBeaconWatcher: TwclBluetoothLeBeaconWatcher
    OnAdvertisementUuidFrame = wclBluetoothLeBeaconWatcherAdvertisementUuidFrame
    OnStarted = wclBluetoothLeBeaconWatcherStarted
    OnStopped = wclBluetoothLeBeaconWatcherStopped
    Left = 344
    Top = 120
  end
end
