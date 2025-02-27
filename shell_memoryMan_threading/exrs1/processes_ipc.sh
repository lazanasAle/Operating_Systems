#!/bin/bash

##############################################################################################################################################################################################################################################
# Alexios Lazanas,  1100605
# Stamatia Kalimeri, 1103080
##############################################################################################################################################################################################################################################

#SOS αλλαγη Change_data πιο απλη η 2η awk με mapping
#Το προγραμμα ολο λειτουργει

#read csv data
insert_data(){
    echo "insert the name of the file"
    read filename
    if [ -z "$filename" ]
    then
        echo "The passenger_data.csv is inserted"
        filename=passenger_data.csv
    else
        echo "The file of your choice is inserted"

    fi
}

search_passenger(){
echo "name or surname of passenger"
read  name
grep $name -w $filename
}

change_data(){

echo "Information inserted"
code=$1

#και αλλες μεταβλητες για ολο το record?

#search and test if code is a number
if [[ $1 =~ ^[0-9]+$ ]]
then
    col=1
else
    echo "please give full_name"
    read code
    col=2

fi

if [ $2 == "record:" ]
then
    awk -F";" -v Code="$code" -v Id=$3 -v Name="$4" -v Surname="$5" -v Age=$6 -v Country="$7" -v Status="$8" -v Rescuded="$9" -v Col=$col 'BEGIN{FS=OFS=";"}
{
    if($Col==Code){
        Fullname=Name " " Surname;
        $1=Id
        $2=Fullname
        $3=Age
        $4=Country
        $5=Status
        $6=Rescuded
    }

}1' passenger_data.csv > tmp.csv
else
echo "Change part of record"
#actual change data
awk -F";" -v Field=$2 -v Change="$3" -v Change1="$4" -v Code="$code" -v Col=$col 'BEGIN{FS=OFS=";"}{
 if(Field == "fullname:")
{
    if($Col==Code)
        {
        Fullname=Change " " Change1;
        $2=Fullname
        }
}
 else if(Field=="age:")
 {
    if($Col==Code)
        {$3=Change}
 }
 else if(Field=="country:")
 {
   if($Col==Code)
        {$4=Change}
 }
 else if(Field=="status:")
 {
   if($Col==Code)
        {$5=Change}
 }
 else if(Field=="rescued:")
 {
    if($Col==Code)
        {$6=Change}
 }
 else
 {
    print "Not given"
 }
}1' passenger_data.csv > tmp.csv
# αυτο θα αλλαξει υπαρχει για τεστ
fi
echo "Showing change...."
cat tmp.csv > passenger_data.csv
rm tmp.csv
grep "$code" passenger_data.csv


}

#show data
display_file(){
    more $filename #press space to view the file page by page
}

#create reports
generate_reports(){
#φιλτραρισμα για διασωθεντες
grep "yes" $filename > rescued.txt

#ευρεση ηλικιακων ομαδων

eof=$(wc -l < passenger_data.csv )
people=1280
awk -F";" -v EOF=$eof -v People=$people '{
    i=0
    grp1
    grp2
    grp3
    grp4
    mo1
    mo2
    mo3
    mo4
    p1
    p2
    p3
    p4
    while (i <= EOF){
    if($1==i){
        if($3 <= 18)
        {
            mo1+=$3
            grp1++
            if($5 == "Crew")
            {
            p1++
            }
        }
        else if($3 >= 19 && $3 <= 35)
        {
            mo2+=$3
            grp2++
            if($5 == "Crew")
            {
            p2++
            }

        }
        else if($3 >= 36 && $3 <= 50)
        {
            mo3+=$3
            grp3++
            if($5 == "Crew")
            {
            p3++
            }

        }
        else
        {
           mo4+=$3
           grp4++
           if($5 == "Crew")
            {
            p4++
            }
        }

    }
    i++
}

}
END{\
print p1," ",p2," ",p3," ",p4
mo1=mo1/grp1
mo2=mo2/grp2
mo3=mo3/grp3
mo4=mo4/grp4
p1=(p1/People)*100
p2=(p2/People)*100
p3=(p3/People)*100
p4=(p4/People)*100
print "The age groups insering in a file called ages.txt:"
print "Group A: 0-18" > "ages.txt"
print "Group B: 19-35" > "ages.txt"
print "Group C: 36-50" > "ages.txt"
print "Group D: 51+" > "ages.txt"

print "the average per group insering in a file called avg.txt:"
print  "The average age of group A:",mo1 > "avg.txt"
print  "The average age of group B:",mo2 > "avg.txt"
print  "The average age of group C:",mo3 > "avg.txt"
print  "The average age of group D:",mo4 > "avg.txt"

print "the precentage per group for helping the rescue insering in a file called percentages.txt:"
print  "The average age of group A:",p1 > "percentages.txt"
print  "The average age of group B:",p2 > "percentages.txt"
print  "The average age of group C:",p3 > "percentages.txt"
print  "The average age of group D:",p4 > "percentages.txt"
}
' passenger_data.csv
}

echo "Welcome to PassFinder...\n"
echo "Inserting data .........\n"
insert_data
echo "displaying Data..."
display_file

#choice=$1
#choice can be code for update or reports
#$2 field or record
echo "Do you want to search someone [Y/N]?"
read search
until [ "$search" == "N" ]
do
search_passenger
echo "Do you want to search someone else [Y/N]?"
read search
done

if [ "$1" == "reports" ]
then
echo "displaying reports\n"
generate_reports
elif [ "$2" == "record:" ] || [ "$2" == "fullname:" ] || [ "$2" == "age:" ] || [ "$2" == "country:" ] || [ "$2" == "status:" ] || [ "$2" == "rescued:" ]
then
echo "update passenger"
change_data $1 $2 $3 $4 $5 $6 $7 $8 $9
else
    echo "Bye!"
fi







