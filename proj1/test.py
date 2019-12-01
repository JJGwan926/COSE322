import csv
path = './nilfs2/nillog.txt'
f = open(path, 'r')
w = open('./nilfs2log.csv', 'w', newline='')
ww = csv.writer(w)
if f == None:
    print("error\n")
else:
    while True:
        line = f.readline()[:-1]
        if not line:
            break
        token = line.split(' ')
        ww.writerow([token[-3][:-1], token[-1]])

f.close()
w.close()
