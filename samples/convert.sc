# Conversions: type a new number in column B (= on the cell);
# column D shows the result.
format 5 = "%Y-%m-%d %H:%M"
format 6 = "%Y-%m-%d"
format 7 = "%H:%M"
format 8 = "$#,0.00;($#,0.00)"
format 9 = "0.0%"
leftstring A0 = "CONVERSIONS"
rightstring B2 = "Value"
leftstring C2 = " Unit"
rightstring D2 = "Result"
leftstring E2 = " Unit"
let B3 = 100
leftstring C3 = " miles"
let D3 = B3*1.609344
leftstring E3 = " km"
let B4 = 100
leftstring C4 = " km"
let D4 = B4/1.609344
leftstring E4 = " miles"
let B5 = 72
leftstring C5 = " deg F"
let D5 = (B5-32)*5/9
leftstring E5 = " deg C"
let B6 = 20
leftstring C6 = " deg C"
let D6 = B6*9/5+32
leftstring E6 = " deg F"
let B7 = 150
leftstring C7 = " pounds"
let D7 = B7*0.45359237
leftstring E7 = " kg"
let B8 = 70
leftstring C8 = " kg"
let D8 = B8/0.45359237
leftstring E8 = " pounds"
let B9 = 12
leftstring C9 = " inches"
let D9 = B9*2.54
leftstring E9 = " cm"
let B10 = 10
leftstring C10 = " gallons"
let D10 = B10*3.785411784
leftstring E10 = " liters"
let B11 = 30
leftstring C11 = " mpg"
let D11 = 235.214583/B11
leftstring E11 = " l/100km"
let B12 = 5280
leftstring C12 = " feet"
let D12 = B12*0.3048
leftstring E12 = " meters"
format A 2 0 0
format B 10 2 0
format C 9 2 0
format D 10 2 0
format E 9 2 0
goto B3
