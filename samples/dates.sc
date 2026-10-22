# Dates: @now is the current time in seconds.  Change B3 with
# =@dts(year,month,day).
format 5 = "%Y-%m-%d %H:%M"
format 6 = "%Y-%m-%d"
format 7 = "%H:%M"
format 8 = "$#,0.00;($#,0.00)"
format 9 = "0.0%"
leftstring A0 = "DATES"
leftstring A2 = "Now"
let B2 = @now
leftstring A3 = "Date"
let B3 = @dts(2027,1,1)
leftstring A4 = "Days to go"
let B4 = @ceil((B3-B2)/86400)
leftstring A5 = "Weekday"
leftstring B5 = @date(B3,"%A")
leftstring A6 = "+90 days"
let B6 = B2+90*86400
leftstring A7 = "Day of year"
leftstring B7 = @date(B2,"%j")
leftstring A8 = "Week no."
leftstring B8 = @date(B2,"%V")
leftstring A10 = "Hour"
let B10 = @hour(B2)
leftstring A11 = "Minute"
let B11 = @minute(B2)
format A 12 2 0
format B 18 0 0
fmt B2 "%Y-%m-%d %H:%M"
fmt B3 "%Y-%m-%d"
fmt B6 "%Y-%m-%d"
goto B3
