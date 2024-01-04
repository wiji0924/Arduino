
const int FrontTrigPin = 14;
// trig pin = D5
const int FrontEchoPin = 12;
// echo pin = D6

// defines variables
long Frontduration;

float front = 0.0;

void setup() {
  Serial.begin(9600);
  pinMode(FrontTrigPin, OUTPUT); 
  pinMode(FrontEchoPin, INPUT); 
}

void loop() {

  digitalWrite(FrontTrigPin, LOW); 
  delayMicroseconds(2);

  digitalWrite(FrontTrigPin, HIGH);
  delayMicroseconds(20);

  digitalWrite(FrontTrigPin, LOW);

  Frontduration = pulseIn(FrontEchoPin, HIGH);

  float frontdata = Frontduration * 0.034 / 2; 

  Serial.print("Distance: ");
  Serial.print(frontdata);
  Serial.println(" cm");// working  code for aj-sr04m

  delay(100);
}
