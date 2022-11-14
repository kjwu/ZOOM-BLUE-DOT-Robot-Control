void sharpStopMotors(){
     LSpeed = 0;
     RSpeed = 0;
     roboclaw.ForwardM1(address,LSpeed); 
     roboclaw.ForwardM2(address,RSpeed);
}


void stopMotors(int dir){
  
  while(LSpeed>0 ||RSpeed >0){
     LSpeed-=1;
     RSpeed-=1;
     if(dir == 1){
     roboclaw.ForwardM1(address,LSpeed); 
     roboclaw.ForwardM2(address,RSpeed);
     }else if(dir == -1){
     roboclaw.BackwardM1(address,LSpeed); 
     roboclaw.BackwardM2(address,RSpeed);

     }
  }
     LSpeed = 0;
     RSpeed = 0;
     Serial.println("motor stop");

}

void turnForwardLeft(float tspd){
  if(LSpeed>0){
    LSpeed-=1;
  }
  if(RSpeed<tspd){
    RSpeed+=1;
  }

  LSpeed = constrain(LSpeed,0,tspd);
  RSpeed = constrain(RSpeed,0,tspd);

//   Serial.println("forward left");
   roboclaw.ForwardM2(address,LSpeed); 
   roboclaw.ForwardM1(address,RSpeed); 
}

void turnForwardRight(float tspd){
  if(RSpeed>0){
    RSpeed-=1;
  }
  if(LSpeed<tspd){
    LSpeed+=1;
  }
  LSpeed = constrain(LSpeed,0,tspd);
  RSpeed = constrain(RSpeed,0,tspd);
//  Serial.println("forward right");
  roboclaw.ForwardM2(address,LSpeed); 
  roboclaw.ForwardM1(address,RSpeed); 
}


void goForward(float tspd){
  if(LSpeed<tspd){
    LSpeed+=1;
  }

  if(RSpeed<tspd){
    RSpeed+=1;
  }
    LSpeed = constrain(LSpeed,0,tspd);
    RSpeed = constrain(RSpeed,0,tspd);
//    Serial.println("go forward");
    roboclaw.ForwardM2(address,LSpeed); 
    roboclaw.ForwardM1(address,RSpeed); 
}

void turnBackwardLeft(float tspd){
  if(LSpeed>0){
    LSpeed-=1;
  }
  if(RSpeed<tspd){
    RSpeed+=1;
  }
    LSpeed = constrain(LSpeed,0,tspd);
  RSpeed = constrain(RSpeed,0,tspd);
//   Serial.println("backward left");
   roboclaw.BackwardM2(address,LSpeed); 
   roboclaw.BackwardM1(address,RSpeed); 
}

void turnBackwardRight(float tspd){
  if(RSpeed>0){
    RSpeed-=1;
  }
  if(LSpeed<tspd){
    LSpeed+=1;
  }
  LSpeed = constrain(LSpeed,0,tspd);
  RSpeed = constrain(RSpeed,0,tspd);
//  Serial.println("backward right");
  roboclaw.BackwardM2(address,LSpeed); 
  roboclaw.BackwardM1(address,RSpeed); 
}


void goBackward(float tspd){
  if(LSpeed<tspd){
    LSpeed+=1;
  }

  if(RSpeed<tspd){
    RSpeed+=1;
  }
    LSpeed = constrain(LSpeed,0,tspd);
    RSpeed = constrain(RSpeed,0,tspd);
//    Serial.println("go backward");
    roboclaw.BackwardM2(address,LSpeed); 
    roboclaw.BackwardM1(address,RSpeed); 
}
