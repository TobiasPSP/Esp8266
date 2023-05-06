# XL6009

The regulator chip *XL6009* is a popular high-power *buck and boost* converter with a claimed maximum currency of **4A**. Today, it is often replaced by its successor *XL6019* which features a maximum currency of **5A*.

> Whether a board with *XL6009* works as a buck, boost, or book-and-boost converter depends on the board design. There are cheap boards for around €0.70/piece that do a one-way boost as well as more sophisticated boards for around €2 that do buck and boost.
> Typically, boards with just *one* coil element go just one way, and boards with *two* coil elements do buck and boost.

| Input | Output | Maximum Current | Switching Frequency | Cost for complete board |
| --- | --- | --- | --- | --- |
| 3.2-32V | 4-38V | 4A | 400kHz | low (€ 0.70/piece for boost-only, € 2 for buck-boost) |

[Datasheet](https://www.haoyuelectronics.com/Attachment/XL6009/XL6009-DC-DC-Converter-Datasheet.pdf)

## Real World Considerations

The true maximum output current depends on the voltages you input and output. Generally speaking, if you require **>10W** output (or **>2.5A**), you need to add a heat sink to the chip.

Here are some realistic estimates:

| Boost | Max Current |
| --- | --- |
| 3V > 12V | 400 mA 4.8W |
| 5V > 12V | 800 mA 9.6W |
| 7.4V > 12V | 1.500 mA 18W |
| 12V > 15V | 2.000 mA 30W |
| 12V > 16V | 2.000 mA 32W |
| 12V > 18V | 1.600 mA 28.8W |
| 12V > 19V | 1.500 mA 28.5W |
| 12V > 24V | 1.000 mA 24W |
