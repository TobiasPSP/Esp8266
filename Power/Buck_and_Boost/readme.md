# Power Supplies in Electronics

Most if not all electonic projects need power supplies, whether discrete or microcontroller-based. Power supplies take an input voltage and transform it to the required output voltage that your project needs.

In the old days, old-fashioned expensive and heavy transformers were used for this, and they still are when you want to convert high voltage **AC** (i.e. from your household sockets) to low-voltage **DC**. Most of the time, though - and especially when you deal with **DC** input - electronic "switching* supplies are used today.

There are two different basic concepts in use today: *Buck* and *Boost*. 

## Electronic Switching Supplies

In the past, voltage was *transformed* to different levels by use of transformers. They used two separate coils with different numbers of windings to increase or decrease voltage through magnetic fields. 

Aside from the fact that transformers are heavy and expensive, they also only work with AC (not DC) because they need magnetic fields, and are inflexible. You cannot simply change the output voltage without changing the number of windings in the coil.

Electronic switching power supplies go a different route and increase or decrease voltage by use of high-frequency switching. To increase voltage, for example, capacitors are used to charge up in one cycle, then submit their energy. If this is done in high frequency, the voltage can be increased almost arbitrarily (at the expense of current which is lowered). Switching power supplies work with **DC** only.

Switching power supplies have a number of advantages:

* **Flexible**: they can increase or decrease voltage, and you can adjust the voltaqge using a simple resistor (or potentiometer)
* **Cheap**: since they just need a couple of discrete components and an IC (the regulator), they are cheap to manufacture, small and light-weight.
* **Current Control**: in addition to voltage regulation, they *can* (not all do) also limit *current* to a maximum. This is called *constant current* (**CC**, versus *constant-voltage* **CV**) and is achieved by increasing the voltage until the specified current is drawn.

## Buck-Converters

*Buck* converters step down the voltage. You need these when the input voltage is higher than what you need. For example, if you want to run a microcontroller-based project at 5V off your car battery, you need a *buck* converter with an input range that covers 10-14V and produces a stable 5V output voltage from it.

> Note that some *buck* converters need an input voltage at least 2V higher than the desired output voltage.

## Boost Converters

*Boost converters* step up the voltage. You need these when the input voltage is lower than what you need. For example, if you run a project from two AA batteries but need 5V for a microcontroller, you can use a *boost* converter to produce that voltage.
 
## Buck-Boost-Converters

These are the most flexible converters as they can both raise and lower the input voltage to the desired voltage. Technically, they are just combined book and boost converters, and depending on the input voltage, one of the two concepts kicks in. 

Combined *buck-boost* converters are perfect if you want to drive your projects from a wide range of input voltage. I.e., if you want to run your project both from 12V car supplies and 3V AA batteries, a combined controller will suit the need.


