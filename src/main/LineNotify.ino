void LineNoti(int param,int check){
    /*if (distance <= 10 && distance != 0 ) {
      line.message = "แหะๆมีคนเข้าบ้านโว่ย";
      LineNotifySendingResult result = LineNotify.send(line);
    }*/
    Serial.println("Notiline");
    Serial.println(param);
    static unsigned long last_notify_time = 0;
    unsigned long current_time = millis();
    if (current_time - last_notify_time >= 1000) {
    last_notify_time = current_time;
      if (check == 3) {
        line.message = "มีคนเข้าบ้านมาเพิ่มทั้งหมด "+String(param)+" คนแล้วงับ";
        LineNotifySendingResult result = LineNotify.send(line);
      }
      if (param <= 10 && check == 1 ) {
        line.message = "โอ้ยๆมืดแล้วเปิดไฟหน่อยดิๆๆๆๆๆ "+String(param)+" lux";
        LineNotifySendingResult result = LineNotify.send(line);
      }
      if (param > 10 && check == 1 ){
        line.message = "ยังสว่างอยู่อย่าเพิ่งเปิดไฟ!! ประหยัดไฟหน่อยเถอะ "+String(param)+" lux";
        LineNotifySendingResult result = LineNotify.send(line);
        }
      
      if (param < 30 && check == 2 ){
        line.message = "ตอนนี้อากาศกำลังดี"+String(param)+" C";
        LineNotifySendingResult result = LineNotify.send(line);
        }
      if (param >= 30 && check == 2 ){
        line.message = "โอ้ยร้อนๆๆๆ ร้อนไม่ไหวละ"+String(param)+" C";
        LineNotifySendingResult result = LineNotify.send(line);
        }
      if (param >=40 && param <=60 && check == 4 ){
      line.message = "ค่าความชื้นปกตินะงับอยู่ที่ "+String(param)+"%";
      LineNotifySendingResult result = LineNotify.send(line);
      }
      if (param > 60 && check == 4 ){
      line.message = "ค่าความชื้นสูงงงง "+String(param)+"%";
      LineNotifySendingResult result = LineNotify.send(line);
      }
      if (param < 40 && check == 4 ){
      line.message = "ค่าความชื้นต่ำง่ะ "+String(param)+"%";
      LineNotifySendingResult result = LineNotify.send(line);
      }
    }
}
