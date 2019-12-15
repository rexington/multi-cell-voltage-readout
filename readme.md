# Multi-cell voltage readout

This is a basic readme for now - may make it more complete at some point (or not).

Here's [the spreadsheet](https://docs.google.com/spreadsheets/d/1-ukImf4vn2myiOkAQJQwKdqlvT10J66j5Gh3QOcvFNc/edit?usp=sharing) I used to figure out the voltage dividers.

I'm sure there's a better way for figuring out what the max voltage is the divider will handle, but I just increased the number until I had exactly 5v on the `Vout` column for a given divider. I then used that number for the map function in my code.