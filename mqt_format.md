# konteks: minta info pasien
- publisher: alat 
- topik: data_request
- format pesan: 
{
    "request_time": 757233,
    "device_id": "mirocle_001",
    "request_type": "info_pasien"
}

# konteks: balas permintaan info pasien
- publisher: server
- topik: data_reply
- format pesan:
{
  "device_id": [seri_alat],
  "reply_type": [tipe_balasan],
  "request_time": [waktu_permintaan],
  "nama": [nama_pasien],
  "jenis_kelamin": [jenis_kelamin],
  "berat_badan": [berat_badan_pasien],
  "usia": [usia_pasien]
}
- contoh pesan:
{
  "device_id": "mirocle_001",
  "reply_type": "info_pasien",
  "request_time": 9320080,
  "nama": "Yanwardo",
  "jenis_kelamin": 1,
  "berat_badan": 55,
  "usia": 40
}

# koneks: kirim info mulai terapi
- publisher: alat
- format topik: terapi_begin 
- contoh topik: terapi_begin
- format pesan: 
{
    "seri_alat": [seri_alat],
    "id_terapi": [unixtime]
}
- contoh pesan: 
{
    "seri_alat": "mirocle_001",
    "id_terapi": 1682894210
}

# konteks: kirim data terapi
- publisher: alat
- format topik: terapi_data/[id_terapi]
- contoh topik: terapi_data/1682894210
- format pesan: 
{
    "detakJantung":[nilai],
    "jumlahDetakJantung":[nilai],
    "saturasiOksigen":[nilai],
    "kalori":[nilai],
    "putaranPedal":[nilai]
}
- contoh pesan:
{
    "detakJantung":0,
    "jumlahDetakJantung":0,
    "saturasiOksigen":0,
    "kalori":33.79529953,
    "putaranPedal":0
}

# konteks: kirim data final terapi
- publisher: alat
- format topik: terapi_end
- contoh topik: terapi_end
- format pesan: 
{
    "id_terapi": 1682894210,
    "detakJantungRataRata":[nilai],
    "kaloriTotal":[nilai]
}
- contoh pesan:
{
    "id_terapi": [unixtime],
    "detakJantungRataRata":0,
    "kaloriTotal":3.87174496e8
}
 

