# A monthly budget: change the Budget and Actual numbers (= on a cell).
# Diff and % used recalculate.
format 5 = "%Y-%m-%d %H:%M"
format 6 = "%Y-%m-%d"
format 7 = "%H:%M"
format 8 = "$#,0.00;($#,0.00)"
format 9 = "0.0%"
leftstring A0 = "MONTHLY BUDGET"
leftstring A2 = "Item"
rightstring B2 = "Budget"
rightstring C2 = "Actual"
rightstring D2 = "Diff"
rightstring E2 = "% used"
leftstring A3 = "Rent"
let B3 = 1200
let C3 = 1200
let D3 = B3-C3
let E3 = C3/B3
leftstring A4 = "Groceries"
let B4 = 450
let C4 = 512.37
let D4 = B4-C4
let E4 = C4/B4
leftstring A5 = "Utilities"
let B5 = 180
let C5 = 164.2
let D5 = B5-C5
let E5 = C5/B5
leftstring A6 = "Car"
let B6 = 300
let C6 = 342.15
let D6 = B6-C6
let E6 = C6/B6
leftstring A7 = "Phone"
let B7 = 65
let C7 = 65
let D7 = B7-C7
let E7 = C7/B7
leftstring A8 = "Books"
let B8 = 60
let C8 = 94.99
let D8 = B8-C8
let E8 = C8/B8
leftstring A9 = "Eating out"
let B9 = 120
let C9 = 88.5
let D9 = B9-C9
let E9 = C9/B9
leftstring A11 = "Total"
let B11 = @sum(B3:B9)
let C11 = @sum(C3:C9)
let D11 = B11-C11
let E11 = C11/B11
leftstring A13 = "Biggest overrun"
let D13 = @min(D3:D9)
leftstring A14 = "Items over budget"
let D14 = (C3>B3)+(C4>B4)+(C5>B5)+(C6>B6)+(C7>B7)+(C8>B8)+(C9>B9)
fmt D14 "0"
format A 12 2 0
format B 11 2 8
format C 11 2 8
format D 11 2 8
format E 8 1 9
goto B3
