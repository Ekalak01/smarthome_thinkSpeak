void LineNoti(int distance,int light,float temp){
    /*if (distance <= 10 && distance != 0 ) {
      line.message = "แหะๆมีคนเข้าบ้านโว่ย";
      LineNotifySendingResult result = LineNotify.send(line);
    }*/
    line.message = "มีคนเข้าบ้านมา "+String(totalPersons)+" แล้วงับ";
    LineNotifySendingResult result = LineNotify.send(line);
    if (light <= 10) {
      line.message = "โอ้ยๆมืดแล้วเปิดไฟหน่อยดิๆๆๆๆๆ";
      LineNotifySendingResult result = LineNotify.send(line);
    }
    if (light >= 20){
      line.message = "ยังสว่างอยู่อย่าเพิ่งเปิดไฟ!! ประหยัดไฟหน่อยเถอะ";
      LineNotifySendingResult result = LineNotify.send(line);
      }
    if (temp < 30){
      line.message = "ตอนนี้อากาศกำลังดี"+String(temp,2)+" C";
      LineNotifySendingResult result = LineNotify.send(line);
      }
    if (temp >= 30){
      line.message = "โอ้ยร้อนๆๆๆ ร้อนไม่ไหวละ"+String(temp,2)+" C";
      LineNotifySendingResult result = LineNotify.send(line);
      }
  
}
