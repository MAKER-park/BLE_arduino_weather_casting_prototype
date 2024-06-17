import 'package:flutter/material.dart';
import 'package:flutter_blue/flutter_blue.dart';
import 'package:permission_handler/permission_handler.dart';

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
  BluetoothCharacteristic? _characteristic;
  List<BluetoothDevice> _devicesList = [];
  bool _isScanning = false;

  @override
  void initState() {
    super.initState();
    _requestPermissions().then((_) {
      _scanForDevices();
    });
  }

  Future<void> _requestPermissions() async {
    if (await Permission.bluetoothScan.request().isGranted &&
        await Permission.bluetoothConnect.request().isGranted &&
        await Permission.location.request().isGranted) {
      // Permissions granted
    }
  }

  void _scanForDevices() {
    if (_isScanning) {
      flutterBlue.stopScan();
    }

    setState(() {
      _isScanning = true;
    });

    flutterBlue.startScan(timeout: Duration(seconds: 4)).catchError((error) {
      print("Error starting scan: $error");
    });

    flutterBlue.scanResults.listen((results) {
      setState(() {
        _devicesList = results.map((result) => result.device).toList();
        _isScanning = false;
      });
    });

    flutterBlue.isScanning.listen((isScanning) {
      setState(() {
        _isScanning = isScanning;
      });
    });
  }

  void _connectToDevice(BluetoothDevice device) async {
    await device.connect();
    _device = device;

    List<BluetoothService> services = await device.discoverServices();
    services.forEach((service) {
      if (service.uuid.toString() == "0000180c-0000-1000-8000-00805f9b34fb") {
        service.characteristics.forEach((characteristic) {
          if (characteristic.uuid.toString() == "00002a56-0000-1000-8000-00805f9b34fb") {
            _characteristic = characteristic;
          }
        });
      }
    });

    setState(() {}); // Update the UI after connection
  }

  void _sendMessage(bool message) async {
    if (_characteristic != null) {
      await _characteristic!.write([message ? 1 : 0]);
    }
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
              onPressed: _scanForDevices,
              child: Text(_isScanning ? 'Scanning...' : 'Scan for Devices'),
            ),
            SizedBox(height: 20),
            Expanded(
              child: ListView.builder(
                itemCount: _devicesList.length,
                itemBuilder: (context, index) {
                  return ListTile(
                    title: Text(_devicesList[index].name),
                    subtitle: Text(_devicesList[index].id.toString()),
                    onTap: () => _connectToDevice(_devicesList[index]),
                  );
                },
              ),
            ),
            SizedBox(height: 20),
            if (_device != null) ...[
              Text('Connected to ${_device!.name}'),
              ElevatedButton(
                onPressed: () => _sendMessage(true),
                child: Text('Send ON Message'),
              ),
              ElevatedButton(
                onPressed: () => _sendMessage(false),
                child: Text('Send OFF Message'),
              ),
            ],
          ],
        ),
      ),
    );
  }
}
