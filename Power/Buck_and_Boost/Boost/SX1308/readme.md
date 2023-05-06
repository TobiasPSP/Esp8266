# SX1308

The regulator chip *SX1308* is a small and cheap boost converter with the following specs:

| Input | Output | Maximum Current | Switching Frequency | Cost for complete board |
| --- | --- | --- | --- | --- |
| 2-24V | 2-28V | 2A | 1.2MHz | low (€ 0.40/piece) |

## Real World Evaluation

The claims for this chip often do not match reality:

* The maximum output current will not reach 2A. Instead, the output voltage starts to collapse, and the efficiency decreases with increasing current (input current increases disporportionally). 
* A more realistic maximum current would probably be around 0.5-1.4A depending on input and output voltages. 
* Typically, low input voltages will provide the least maximum output current (as expected)
* The higher the delta between input and output voltage, the earlier will you see a voltage drop
* The output voltage is not very stable and drops finally when current is too high
* Input voltage needs to be 3V at minimum

This chip *can* be a good (cheap) way to go for some scenarios but you might want to test first with the particular input and output voltages you need. [Here](https://www.matts-electronics.com/wp-content/uploads/2018/06/SX1308.pdf) are some good graphs to guide you.


