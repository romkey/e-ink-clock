class Uptime {
 public:
  Uptime() { start = millis(); }
  unsigned long uptime() { return millis() - start; }
 private:
  unsigned long start;
};
