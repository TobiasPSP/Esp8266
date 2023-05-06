# SX1308

The regulator chip *SX1308* is a small and cheap boost converter with the following specs:

| Input | Output | Maximum Current | Switching Frequency | Cost for complete board |
| --- | --- | --- | --- | --- |
| 2-24V | 2-28V | 2A | 1.2MHz | low (€ 0.40/piece) |

[Datasheet](https://datasheet.lcsc.com/szlcsc/Shenzhen-Suosemi-Tech-SX1308_C78162.pdf)

> While you can buy this boost converter as a fully-functional PCB at some online stores for as much as €6 per piece, the real retail price for the very same product at places like *AliExpress* is around € 0.40, most often sold in packs of 10 pieces.

## Real World Evaluation

The claims for this chip often do not match reality:

* The maximum output current *will not reach 2A*. Instead, the meximum output current significantly depends on the input and output voltages you need and will never dependably exceed 1.2A. For relatively small input voltages of i.e. 3V and relatively large output voltages of 12V (low input voltage and large voltage delta), maximum current can be less than 500mA.
* When the maximum current is reached, output voltage will start to drop, and conversion efficiency will drop as well.
* This all is most likely happening due to an internal current capture at **input** current.
* the output voltage starts to collapse, and the efficiency decreases with increasing current (input current increases disporportionally). 
* Even within current specs, output voltage is not very stable and may vary by some hundred mV.
* Input voltage needs to be 3V at minimum


This chip *can* be a good (cheap) way to go for some scenarios but you might want to test first with the particular input and output voltages you need. [Here](https://www.matts-electronics.com/wp-content/uploads/2018/06/SX1308.pdf) are some good graphs to guide you, and some realistic estimates:

| Boost | Max Current |
| --- | --- |
| 3.7V > 12V | 450mA |
5V > 12V | 700mA |
12V > 19V | 1.2A |
12V > 24V | 800mA |

Within these ranges, use of this chip may be a very reasonable way to go.

