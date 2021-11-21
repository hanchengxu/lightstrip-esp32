
boolean isClearDay(int weatherCode){
  int clearDayCodes[12] = {100, 101, 110, 111, 130, 131, 132, 200, 201,210,211,223};
  int i, thisindex = -1;

  for(i=0; i<12; i++){
        if(clearDayCodes[i] == weatherCode){
            thisindex = i;
            break;
        }else if(clearDayCodes[i] > weatherCode){
            break;
        }
    }
  return  thisindex < 0 ? false : true;
}
