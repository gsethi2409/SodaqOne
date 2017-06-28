bool resetLora() {
  debugPrintln("Hard reseting");
  pinMode(LORA_RESET, OUTPUT);
  digitalWrite(LORA_RESET, HIGH);
  sodaq_wdt_safe_delay(100);
  
  
  digitalWrite(LORA_RESET, LOW);
  sodaq_wdt_safe_delay(100);
  digitalWrite(LORA_RESET, HIGH);
  sodaq_wdt_safe_delay(1000);
  initLora();
}

/*For V2, connect RESET pin to Ground while board is powered*/
