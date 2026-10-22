D = "\x04"
FMTS = f'''format 5 = "{D}%Y-%m-%d %H:%M"
format 6 = "{D}%Y-%m-%d"
format 7 = "{D}%H:%M"
format 8 = "$#,0.00;($#,0.00)"
format 9 = "0.0%"
'''
def L(c,s): return f'leftstring {c} = "{s}"'
def R(c,s): return f'rightstring {c} = "{s}"'

# ---- budget
b = ['# A monthly budget: change the Budget and Actual numbers (= on a cell).',
     '# Diff and % used recalculate.', FMTS.strip(),
     L('A0','MONTHLY BUDGET'),
     L('A2','Item'), R('B2','Budget'), R('C2','Actual'), R('D2','Diff'), R('E2','% used')]
items = [('Rent',1200,1200),('Groceries',450,512.37),('Utilities',180,164.20),
         ('Car',300,342.15),('Phone',65,65),('Books',60,94.99),('Eating out',120,88.50)]
r = 3
for name,bud,act in items:
    b += [L(f'A{r}',name), f'let B{r} = {bud}', f'let C{r} = {act}',
          f'let D{r} = B{r}-C{r}', f'let E{r} = C{r}/B{r}']
    r += 1
last = r-1
b += [L(f'A{r+1}','Total'), f'let B{r+1} = @sum(B3:B{last})', f'let C{r+1} = @sum(C3:C{last})',
      f'let D{r+1} = B{r+1}-C{r+1}', f'let E{r+1} = C{r+1}/B{r+1}',
      L(f'A{r+3}','Biggest overrun'), f'let D{r+3} = @min(D3:D{last})',
      L(f'A{r+4}','Items over budget'), f'let D{r+4} = ' + '+'.join(f'(C{i}>B{i})' for i in range(3,last+1)),
      f'fmt D{r+4} "0"',
      'format A 12 2 0', 'format B 11 2 8', 'format C 11 2 8', 'format D 11 2 8', 'format E 8 1 9',
      'goto B3']
open('BUDGET.SC','w').write('\n'.join(b)+'\n')

# ---- loan
l = ['# A loan: change the amount, rate or years in B2..B4 and everything',
     '# below recalculates.  @pmt is the monthly payment.', FMTS.strip(),
     L('A0','CAR LOAN'),
     L('A2','Amount'), 'let B2 = 25000',
     L('A3','Rate/year'), 'let B3 = 0.065',
     L('A4','Years'), 'let B4 = 5',
     L('A5','Payment'), 'let B5 = @pmt(B2,B3/12,B4*12)',
     L('A6','Total paid'), 'let B6 = B5*B4*12',
     L('A7','Interest'), 'let B7 = B6-B2',
     R('A9','Month'), R('B9','Interest'), R('C9','Principal'), R('D9','Balance')]
first = 10
for m in range(1,13):
    rr = first+m-1
    prev = 'B2' if m == 1 else f'D{rr-1}'
    l += [f'let A{rr} = {m}' if m == 1 else f'let A{rr} = A{rr-1}+1',
          f'let B{rr} = {prev}*$B$3/12'.replace('$',''),
          f'let C{rr} = B5-B{rr}',
          f'let D{rr} = {prev}-C{rr}']
l += ['fmt B3 "0.00%"', 'fmt B4 "0"', 'format A 11 0 0', 'format B 11 2 8', 'format C 11 2 8', 'format D 12 2 8', 'goto B2']
open('LOAN.SC','w').write('\n'.join(l)+'\n')

# ---- dates
d = ['# Dates: @now is the current time in seconds.  Change B3 with', '# =@dts(year,month,day).', FMTS.strip(),
     L('A0','DATES'),
     L('A2','Now'), 'let B2 = @now',
     L('A3','Date'), 'let B3 = @dts(2027,1,1)',
     L('A4','Days to go'), 'let B4 = @ceil((B3-B2)/86400)',
     L('A5','Weekday'), 'leftstring B5 = @date(B3,"%A")',
     L('A6','+90 days'), 'let B6 = B2+90*86400',
     L('A7','Day of year'), 'leftstring B7 = @date(B2,"%j")',
     L('A8','Week no.'), 'leftstring B8 = @date(B2,"%V")',
     L('A10','Hour'), 'let B10 = @hour(B2)',
     L('A11','Minute'), 'let B11 = @minute(B2)',
     'format A 12 2 0', 'format B 18 0 0',
     'fmt B2 "' + D + '%Y-%m-%d %H:%M"', 'fmt B3 "' + D + '%Y-%m-%d"', 'fmt B6 "' + D + '%Y-%m-%d"',
     'goto B3']
open('DATES.SC','w').write('\n'.join(d)+'\n')

# ---- conversions
c = ['# Conversions: type a new number in column B (= on the cell);', '# column D shows the result.', FMTS.strip(),
     L('A0','CONVERSIONS'),
     R('B2','Value'), L('C2',' Unit'), R('D2','Result'), L('E2',' Unit')]
conv = [('miles',100,'B*1.609344','km'),('km',100,'B/1.609344','miles'),
        ('deg F',72,'(B-32)*5/9','deg C'),('deg C',20,'B*9/5+32','deg F'),
        ('pounds',150,'B*0.45359237','kg'),('kg',70,'B/0.45359237','pounds'),
        ('inches',12,'B*2.54','cm'),('gallons',10,'B*3.785411784','liters'),
        ('mpg',30,'235.214583/B','l/100km'),('feet',5280,'B*0.3048','meters')]
r = 3
for u1,v,f,u2 in conv:
    c += [f'let B{r} = {v}', L(f'C{r}',' '+u1), f'let D{r} = ' + f.replace('B',f'B{r}'), L(f'E{r}',' '+u2)]
    r += 1
c += ['format A 2 0 0', 'format B 10 2 0', 'format C 9 2 0', 'format D 10 2 0', 'format E 9 2 0', 'goto B3']
open('CONVERT.SC','w').write('\n'.join(c)+'\n')
