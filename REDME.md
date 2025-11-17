🏠 Smart Home Automation System

Sistem otomasi rumah berbasis Arduino + C++ yang mengontrol lampu melalui relay, membaca suhu & kelembaban dari sensor DHT22, dan menerima perintah melalui IR Remote atau Bluetooth (HC-05).
Termasuk fitur Mode Hemat Energi Otomatis yang menyesuaikan penggunaan daya berdasarkan kondisi lingkungan.


---

✨ Fitur Utama

🔌 Kontrol Lampu (Relay 220V / 5V)

🌡️ Sensor Suhu & Kelembaban DHT22

🎮 Kontrol IR Remote

📱 Kontrol Bluetooth via HC-05

⚡ Energy Saver Mode (otomatis matikan lampu jika suhu rendah)

📊 Monitoring realtime di Serial Monitor

🧠 Logika dan state machine terstruktur



---

🧩 Hardware yang Dibutuhkan

Komponen	Jumlah	Keterangan

Arduino Uno / Nano	1	Microcontroller utama
Relay 1 Channel	1	Kontrol lampu / perangkat AC
Sensor DHT22	1	Monitoring suhu & kelembaban
IR Receiver (VS1838B)	1	Input remote
Bluetooth HC-05	Opsional	Kendali via smartphone
Kabel jumper	-	Koneksi
Lampu beban	1	Melalui relay



---

🔌 Wiring Diagram (Ringkas)

Arduino	Komponen

D2	DHT22 Data
D8	Relay IN
D7	IR Receiver OUT
TX/RX	HC-05 RX/TX
5V & GND	Semua modul


Keterangan lengkap bisa ditambah gambar wiring (.png) jika diperlukan.
