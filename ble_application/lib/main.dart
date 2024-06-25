import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:flutter_blue/flutter_blue.dart';
import 'package:permission_handler/permission_handler.dart';
import 'dart:async';

void main() {
  WidgetsFlutterBinding.ensureInitialized();
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter BLE Communication',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: MyHomePage(),
    );
  }
}

class MyHomePage extends StatefulWidget {
  @override
  _MyHomePageState createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  FlutterBlue flutterBlue = FlutterBlue.instance;
  BluetoothDevice? _device;
  BluetoothCharacteristic? _characteristic; // 송신 특성
  BluetoothCharacteristic? _receiveCharacteristic; // 수신 특성 추가
  bool _isBluetoothEnabled = false;
  bool _isConnected = false;
  Timer? _connectionCheckTimer;

  @override
  void initState() {
    super.initState();
    _checkBluetoothStatus();
    _startConnectionCheck();
  }

  @override
  void dispose() {
    _connectionCheckTimer?.cancel();
    super.dispose();
  }

  Future<void> _checkBluetoothStatus() async {
  var status = await Permission.bluetooth.status;
  if (!status.isGranted) {
    status = await Permission.bluetooth.request();
  }

  var locationStatus = await Permission.locationWhenInUse.status;
  if (!locationStatus.isGranted) {
    locationStatus = await Permission.locationWhenInUse.request();
  }

  setState(() {
    _isBluetoothEnabled = status.isGranted && locationStatus.isGranted;
  });

  if (status.isPermanentlyDenied || locationStatus.isPermanentlyDenied) {
    openAppSettings();
  }
}


  void _navigateToScanScreen() async {
    if (_isBluetoothEnabled) {
      print("블루투스 화면 진입");
      final selectedDevice = await Navigator.push(
        context,
        MaterialPageRoute(builder: (context) => ScanScreen()),
      );

      if (selectedDevice != null) {
        _connectToDevice(selectedDevice);
      }
    } else {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('블루투스 기능을 켜주세요.')),
      );
    }
  }

  void _connectToDevice(BluetoothDevice device) async {
    try {
      await device.connect();
      print('장치에 연결되었습니다: ${device.name}');
      _device = device;

      // 연결 후 서비스 발견
      _discoverServices();
    } catch (e) {
      print('장치에 연결하는 중 오류 발생: $e');
    }
  }

  Future<void> _discoverServices() async {
    if (_device == null) {
      print('장치가 연결되지 않았습니다.');
      return;
    }

    try {
      List<BluetoothService> services = await _device!.discoverServices();
      for (var service in services) {
        print('발견된 서비스: ${service.uuid}');
        for (var characteristic in service.characteristics) {
          print('발견된 특성: ${characteristic.uuid}');
        }
      }

      // 서비스와 특성을 찾고 설정하는 로직
      for (var service in services) {
        if (service.uuid.toString() == "0000ffe0-0000-1000-8000-00805f9b34fb") {
          print('타겟 서비스 발견: ${service.uuid}');
          for (var characteristic in service.characteristics) {
            if (characteristic.uuid.toString() == "0000ffe1-0000-1000-8000-00805f9b34fb") {
              print('타겟 특성 발견: ${characteristic.uuid}');
              if (characteristic.properties.write) {
                print('쓰기 가능한 특성입니다.');
                setState(() {
                  _characteristic = characteristic;
                  _isConnected = true;
                });
                print('타겟 특성을 설정하였습니다: ${characteristic.uuid}');
              }
              if (characteristic.properties.notify) {
                print('알림 가능한 특성입니다.');
                _receiveCharacteristic = characteristic;
                await _receiveCharacteristic!.setNotifyValue(true);
                _receiveCharacteristic!.value.listen((value) {
                  print('수신된 데이터: ${utf8.decode(value)}');
                });
              }
            }
          }
        }
      }
      print('타겟 서비스 또는 특성을 찾을 수 없습니다.');
    } catch (e) {
      print('서비스를 발견하는 중 오류 발생: $e');
    }
  }

  void _sendMessage(String message) async {
    print('버튼 클릭 : $message');
    if (_characteristic != null) {
      try {
        await _characteristic!.write(utf8.encode(message + "\n"));
        print('메시지 전송 성공: $message');
      } catch (e) {
        print('메시지 전송 중 오류 발생: $e');
      }
    } else {
      print('특성을 찾을 수 없습니다.');
    }
  }

  void _startConnectionCheck() {
    _connectionCheckTimer = Timer.periodic(Duration(seconds: 10), (timer) async {
      if (_device != null) {
        var connectedDevices = await flutterBlue.connectedDevices;
        bool isConnectedNow = connectedDevices.contains(_device);

        if (isConnectedNow != _isConnected) {
          setState(() {
            _isConnected = isConnectedNow;
          });
        }
      } else {
        setState(() {
          _isConnected = false;
        });
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Flutter BLE Communication'),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            ElevatedButton(
              onPressed: _isBluetoothEnabled && !_isConnected ? _navigateToScanScreen : null,
              child: Text('장치 스캔'),
            ),
            if (!_isBluetoothEnabled)
              Text('블루투스 기능을 켜주세요.'),
            if (_isConnected) ...[
              Expanded(
                child: GridView.count(
                  crossAxisCount: 3, // 그리드의 열 수를 3로 설정
                  crossAxisSpacing: 5.0,
                  mainAxisSpacing: 5.0,
                  padding: EdgeInsets.all(10.0),
                  children: [
                    // 버튼에 표시될 텍스트 목록
                    '눈 온도 높다', '눈 온도 낮다', '비 온도 높다', '비 온도 낮다',
                    '맑음 온도 높다', '맑음 온도 낮다', '흐림 온도 높다', '흐림 온도 낮다',
                    '바람 온도 높다', '바람 온도 낮다'
                  ].asMap().entries.map((entry) {
                    int index = entry.key;
                    String name = entry.value;

                    // 각 버튼을 눌렀을 때 보낼 메시지 목록
                    List<String> messages = [
                      '0', '1', '2', '3',
                      '4', '5', '6', '7',
                      '8', '9'
                    ];

                    return Container(
                      decoration: BoxDecoration(
                        color: Colors.blue,
                        borderRadius: BorderRadius.circular(8.0),
                      ),
                      child: ElevatedButton(
                        style: ElevatedButton.styleFrom(
                          backgroundColor: Colors.blue, // 버튼 색상
                          shape: RoundedRectangleBorder(
                            borderRadius: BorderRadius.circular(8.0),
                          ),
                        ),
                        onPressed: () => _sendMessage(messages[index]),
                        child: Text(
                          name,
                          style: TextStyle(color: Colors.white),
                          textAlign: TextAlign.center, // 텍스트를 중앙 정렬
                        ),
                      ),
                    );
                  }).toList(),
                ),
              ),
            ],
          ],
        ),
      ),
    );
  }
}

class ScanScreen extends StatefulWidget {
  @override
  _ScanScreenState createState() => _ScanScreenState();
}

class _ScanScreenState extends State<ScanScreen> {
  FlutterBlue flutterBlue = FlutterBlue.instance;
  List<BluetoothDevice> _devicesList = [];
  bool _isScanning = false;

  @override
  void initState() {
    super.initState();
    _scanForDevices();
  }

  @override
  void dispose() {
    flutterBlue.stopScan();
    super.dispose();
  }

  void _scanForDevices() {
    if (_isScanning) {
      flutterBlue.stopScan();
    }

    setState(() {
      _isScanning = true;
    });

    flutterBlue.startScan(timeout: Duration(seconds: 4)).catchError((error) {
      print("스캔 시작 중 오류 발생: $error");
    });

    flutterBlue.scanResults.listen((results) {
      if (mounted) {
        setState(() {
          _devicesList = results.map((result) => result.device).toList();
          _isScanning = false;
        });
      }
    });

    flutterBlue.isScanning.listen((isScanning) {
      if (mounted) {
        setState(() {
          _isScanning = isScanning;
        });
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('장치 스캔'),
      ),
      body: Column(
        children: <Widget>[
          ElevatedButton(
            onPressed: _isScanning ? null : _scanForDevices,
            child: Text(_isScanning ? '스캔 중...' : '장치 스캔'),
          ),
          Expanded(
            child: ListView.builder(
              itemCount: _devicesList.length,
              itemBuilder: (context, index) {
                return ListTile(
                  title: Text(_devicesList[index].name),
                  subtitle: Text(_devicesList[index].id.toString()),
                  onTap: () {
                    Navigator.pop(context, _devicesList[index]);
                  },
                );
              },
            ),
          ),
        ],
      ),
    );
  }
}
