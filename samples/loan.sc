# A loan: change the amount, rate or years in B2..B4 and everything
# below recalculates.  @pmt is the monthly payment.
format 5 = "%Y-%m-%d %H:%M"
format 6 = "%Y-%m-%d"
format 7 = "%H:%M"
format 8 = "$#,0.00;($#,0.00)"
format 9 = "0.0%"
leftstring A0 = "CAR LOAN"
leftstring A2 = "Amount"
let B2 = 25000
leftstring A3 = "Rate/year"
let B3 = 0.065
leftstring A4 = "Years"
let B4 = 5
leftstring A5 = "Payment"
let B5 = @pmt(B2,B3/12,B4*12)
leftstring A6 = "Total paid"
let B6 = B5*B4*12
leftstring A7 = "Interest"
let B7 = B6-B2
rightstring A9 = "Month"
rightstring B9 = "Interest"
rightstring C9 = "Principal"
rightstring D9 = "Balance"
let A10 = 1
let B10 = B2*B3/12
let C10 = B5-B10
let D10 = B2-C10
let A11 = A10+1
let B11 = D10*B3/12
let C11 = B5-B11
let D11 = D10-C11
let A12 = A11+1
let B12 = D11*B3/12
let C12 = B5-B12
let D12 = D11-C12
let A13 = A12+1
let B13 = D12*B3/12
let C13 = B5-B13
let D13 = D12-C13
let A14 = A13+1
let B14 = D13*B3/12
let C14 = B5-B14
let D14 = D13-C14
let A15 = A14+1
let B15 = D14*B3/12
let C15 = B5-B15
let D15 = D14-C15
let A16 = A15+1
let B16 = D15*B3/12
let C16 = B5-B16
let D16 = D15-C16
let A17 = A16+1
let B17 = D16*B3/12
let C17 = B5-B17
let D17 = D16-C17
let A18 = A17+1
let B18 = D17*B3/12
let C18 = B5-B18
let D18 = D17-C18
let A19 = A18+1
let B19 = D18*B3/12
let C19 = B5-B19
let D19 = D18-C19
let A20 = A19+1
let B20 = D19*B3/12
let C20 = B5-B20
let D20 = D19-C20
let A21 = A20+1
let B21 = D20*B3/12
let C21 = B5-B21
let D21 = D20-C21
fmt B3 "0.00%"
fmt B4 "0"
format A 11 0 0
format B 11 2 8
format C 11 2 8
format D 12 2 8
goto B2
