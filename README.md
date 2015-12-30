# wristcast
i broke my wrist and wrote software for it.

##photos videos etc

![image of cast](http://i.imgur.com/Nw4IHJz.jpg)

note: someone wonderful covered my wrist in leather, which is why my wrist is covered in leather, and for a time i had a vibrating motor attached to 5v, which buzzed when the button was depressed. i got rid of the vibrating motor. don't attach a vibrating motor.

##i know how to electric and what is this

i don't expect anybody to use this, ever, but please tell me if you do.

the set-up is as follows, glued, soldered and taped up in an amateur fashion:

- an adafruit gemma superglued to my cast
- a neopixel ring superglued to my cast, hooked up to 5v, ground and D1 on the gemma
- a neopixel jewel superglued to my cast, hooked up to 5v, ground and D2 on the gemma
- a momentary switch, pulling D0 on the gemma to ground (code adds a pullup resistor to make the switch function)
- a lipoly battery powering the gemma

plug the gemma on your arm into your usb port like a minor character in a cyberpunk novel, write the sketch in this repo using the ardiuno ide, and power up.

##wiring
wing it. i star-wired to get 3 5v connections and 3 grounds, with super thin wires going down the length of the cast.

##caveats
i had to replace the gemma one time due to the pretty well-known bootloader corruption issue. that sucked, but the replacement worked.

##usage
if you use the sketch as-is, it starts in off-mode. that's on purpose because it's honestly too bright to really consider using around other people.

click the button once to trigger the first mode. the next two clicks increase brightness (it's a really simply debounce - holding the button counts as about 3 clicks per second), and the third click goes to the next mode.

the last mode is a really sweet strobe light, and everyone will hate it. i promise. after that, clicking turns it off again.

it also functions as a watch! hold the button for about 5 seconds and let go when you're clearly in watch-mode (the neopixel ring will change to display something looking like a watch face). orange is the hour hand, green the minute hand, and the blue rotation the second hand.

it's fine-tuned for an 8mhz gemma - holding the button will advance the minute hand very quickly, so you can set the watch (but don't hold the button too long, as this will drop you back into normal mode - set the watch by holding the button in bursts).

yes, you do have to set the watch every time you want to use it. it's not really meant for everyday use. it loses about a minute every two hours. caveat emptor.

##misc
this was one of my first electronics projects so i'm sure i did a tonne of stuff wrong or inefficiently. originally it used a tonne of floating point and trig, but i converted everything to integer math to fit more than one function on the tiny gemma storage. if anyone does actually have suggestions, i'd honestly like to hear them, i got it about as small as i could manage. uses about 98% of the storage.
