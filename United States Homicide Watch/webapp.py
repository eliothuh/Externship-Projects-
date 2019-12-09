#!/usr/bin/env python3
'''
webapp.py sends queries from the frontend to the backend.
It loads and updates pages and processes data in a form easy
for the html to present.
'''

import flask
from flask import render_template, request
import json
import sys
from datasource import *
import psycopg2

app = flask.Flask(__name__)
connection = psycopg2.connect(database="huhe", user="huhe", password="tree695eye")
dataSource = DataSource(connection)


def getStateQueryData(startYear, endYear, state):
	'''
	Returns the average annual rate of homicide in a state (per 100,000 people),
	the national average annual rate of homicide (per 100,000 people),
	a list of rates of homicide over each specified year within the state,
	a list containing each year in the specified range (for our Javascript file), and the
	causes of homicide along with the percentage of total homicides they
	contributed, if accurate data for each said cause is provided.

	PARAMETERS:
		startYear - the first year of data to draw from
		endYear - the last year of data to draw from
		state - the name of the state to draw data from

	RETURN:
		A dictionary containing the average annual rate homicide in the nation and
		state, a list of annual rates of homicide in the state,
		a list of the years in the specified range, and another dictionary storing each cause and the percentage of
		homicides it was responsible for

	Calls getStateCrudeRate, getCausesAndPercentages, getStateSingleYearCrudeRates, getYearRange,
	and getNationalCrudeRate
	'''
	dataTable = {}
	fullList = dataSource.getStateQuery(startYear, endYear, state)

	if isinstance(fullList, Exception):
		raise fullList

	dataTable["yearRange"] = getYearRange(startYear, endYear)
	dataTable["singleYearCrudeRates"] = getStateSingleYearCrudeRates(startYear, endYear, state)

	dataTable["stateCrudeRate"] = getStateCrudeRate(fullList)
	dataTable["causesAndPercentages"] = getCausesAndPercentages(fullList)

	nationTotals = dataSource.getUSATotals(startYear, endYear)
	dataTable["nationalCrudeRate"] = getNationalCrudeRate(nationTotals)

	return dataTable


def getStateSingleYearCrudeRates(startYear, endYear, state):
	'''
	Gets the rate of homicide within the specified state over each year from startYear to endYear,
	places all of these crude rates into a list of ints and returns this list

    PARAMETERS:
    startYear: the first year to find the homicide crude rate for
    endYear: the last year to find the homicide crude rate for
    state: the state to find the homicide crude rate for

    RETURN:
    A list of ints each representing the rate of homicide per 100,000 people in
	each year within the specified range

    Calls getStateCrudeRate
    '''
	list = []
	rate = 0
	crudeRates = []

	for year in range (startYear, endYear + 1):
		list = dataSource.getStateQuery(year, year, state)
		rate = getStateCrudeRate(list)
		crudeRates.append(rate)

	return crudeRates


def getStateCrudeRate(list):
	'''
	Returns the average annual rate of homicide in a state (per 100,000 people) over the
	specified year range. If no data was given over this year range (no population of deaths),
	we return 0.

	PARAMETERS:
		list - an array of state homicide data for each year in the range the user queried

	RETURN:
		A int representing the average annual number of homicides in the user's
		requested state (per 100,000) rounded to 3 decimal places or 0 if no valid data was given

	Calls getAverageStateDeaths, getAverageStatePopulation
	'''
	averageDeaths = getAverageStateDeaths(list)
	averagePopulation = getAverageStatePopulation(list)
	if(averagePopulation == 0):
		return 0
	
	return round(averageDeaths*100000/averagePopulation, 3)


def getAverageStateDeaths(list):
	'''
	Returns the average annual number of homicides in a state (per 100,000 people)

	PARAMETERS:
		list - an array of state homicide data for each year in the range the user queried

	RETURN:
		The average annual number of homicides in the user's requested state (per 100,000)

	Calls getAverageStateDeaths, getAverageStatePopulation
	'''
	tupleIndex = 0;
	stateTotal = 0;
	numYears = len(list)

	for year in list:
		tupleIndex = len(year) - 2
		if(tupleIndex > 0):
			stateTotal += year[tupleIndex][5]

	return stateTotal/numYears


def getAverageStatePopulation(list):
	'''
	Returns the average annual population of the state over user's queried year range

	PARAMETERS:
		list - an array of state homicide data for each year in the range the user queried

	RETURN:
		The average annual population of the user's specified state over the user's
		specified year range
	'''
	numYears = len(list)
	total = 0

	for year in list:
		if(len(year) > 1):
			total += year[0][6]

	return total/numYears


def getYearRange(startYear, endYear):
	'''
	returns a list containing each year (as an int) from startYear to endYear

	PARAMETERS:
	startYear: the first year to store in the list
	endYear: the last year to store in the list

	RETURN:
	A list of ints, starting from startYear and increasing sequentially
	up to and including endYear
	'''
	list = []

	for year in range(startYear, endYear + 1):
		list.append(year)

	return list


def getNationalCrudeRate(list):
	'''
	Returns the national average annual rate of homicide per 100,000 people

	PARAMETERS:
		list - an array of state homicide data for each year in the range the user queried

	RETURN:
		The national average annual rate of homicide per 100,000 people over the
		year range the user queried for

	Calls getNationalAverageDeaths and getAverageNationalPopulation
	'''
	averageDeaths = getNationalAverageDeaths(list)
	averagePopulation = getAverageNationalPopulation(list)

	return round(averageDeaths*100000/averagePopulation, 3)


def getNationalAverageDeaths(list):
	'''
	Returns the average annual number of homicides across the nation

	PARAMETERS:
		list - an array of state homicide data for each year in the range the user queried

	RETURN:
		The national average annual number of homicides
	'''
	total = 0
	tupleIndex = 0
	numYears = len(list)

	for year in list:
		tupleIndex = len(year) - 1
		
		if(tupleIndex > 0):
			total += year[tupleIndex][5]

	return total/numYears


def getAverageNationalPopulation(list):
	'''
	Returns the nation's average population over the user's specified year range

	PARAMETERS:
		list - an array of state homicide data for each year in the range the user queried

	RETURN:
		The national average population over the specified year range
	'''
	numYears = len(list)
	total = 0
	tupleIndex = 0

	for year in list:
		tupleIndex = len(year) - 1

		if(tupleIndex > 0):
			total += year[tupleIndex][6]


	return total/numYears


def getCausesAndPercentages(list):
	'''
	Returns a dictionary with each key being a cause of homicide and each value being the
	percentage of homicides the associated cause was responsible for

	PARAMETERS:
		list - an array of state homicide data for each year in the range the user queried

	RETURN:
		A dictionary with each key being a cause of homicide and each value being the
		percentage of homicides the associated cause was responsible for

	Calls isValidCause, getPercent, and getPercentOther
	'''
	lastIndex = len(list[0]) - 3
	causesList = {}

	for index in range(lastIndex):
		cause = list[0][index][3]
		if(isValidCause(cause, list)):
			causesList[cause] = getPercent(cause, list)

	causesList["Other"] = getPercentOther(causesList, list)

	return causesList


def isValidCause(cause, list):
	'''
	Determines whether the inputted cause has valid data. More specifically, this method
	checks whether the data for this cause was omitted in any of the specified years
	and does not regard it as valid in this case.

	PARAMETERS:
		list - an array of state homicide data for each year in the range the user queried

	RETURN:
		A True value if there was data for this cause every year and a False value otherwise
	'''
	foundAllYears = True

	for year in list:
		foundThisYear = False
		lastIndex = len(year) - 3

		for index in range(lastIndex):
			if(year[index][3] == cause):
				foundThisYear = True

		foundAllYears = foundAllYears and foundThisYear

	return foundAllYears


def getPercent(cause, list):
	'''
	Returns the percentage of total homicides the specified cause of homicide was responsible
	for

	PARAMETERS:
		list - an array of state homicide data for each year in the range the user queried

	RETURN:
		A String representing a number with at most 3 decimal places representing the percentage
		of deaths the specified cause was responsible for
	'''
	totalDeathsByCause = getTotalDeathsByCause(cause, list)
	numberOfYears = len(list)
	totalDeaths = getAverageStateDeaths(list)*numberOfYears

	return round(totalDeathsByCause * 100/totalDeaths, 3)


def getTotalDeathsByCause(cause, list):
	'''
	Returns the total number of deaths the specified cause was responsible for
	over the user's queried year range in the specified state

	PARAMETERS:
		list - an array of state homicide data for each year in the range the user queried

	RETURN:
		An integer representing the total number of homicides the specified cause contributed
	'''
	totalDeaths = 0

	for year in list:
		lastIndex = len(year) - 3

		for index in range(lastIndex):
			if(year[index][3] == cause):
				totalDeaths += year[index][5]

	return totalDeaths


def getPercentOther(causesList, list):
	'''
	Returns the percentage of homicides over the user's queried year range and specified state
	not caused by any of the valid causes already found

	PARAMETERS:
		list - an array of state homicide data for each year in the range the user queried

	RETURN:
		A String representation of a float rounded to 3 decimal places representing the
		percentage of homicides not caused by any of the specified causes
	'''
	percentageKnown = 0

	for cause in causesList:
		percentageKnown += causesList[cause]

	return round(100 - percentageKnown, 3)


def adjustYears(startYear, endYear):
	'''
	Adjusts the start and end years to be the same year if only one is specified
	and sets the start to 1999 and end to 2017 if neither is specified.

	PARAMETERS:
		startYear- the start year specified by the user
		endYear- the ending year specified by the user

	RETURN:
		The start year and end year returned as Strings
	'''
	if(startYear is None):
		startYear = ""

	if(endYear is None):
		endYear = ""

	if(startYear == "" and endYear == ""):
		startYear = "1999"
		endYear = "2017"

	elif(startYear == ""):
		startYear = endYear

	elif(endYear == ""):
		endYear = startYear

	return startYear, endYear


def setYearsToInts(startYear, endYear):
	'''
	Converts the inputted start year and end year to ints.

	PARAMETERS:
		startYear- the starting year for the query passed as a String
		endYear- the ending year for the query passed as a String

	RETURN:
		the start year String converted into an int and the end year String
		converted into an int
	'''

	startYear = int(startYear)
	endYear = int(endYear)

	return startYear, endYear


def cleanStateInput(state):
	'''
	Re-formats the inputted state to be usable in a SQL query. More specifically, this function
	returns a String with the leading and trailing white space of the input removed and each word
	within the string (except conjunctions/prepositions like "of" or "and") capitalized.
	If no string was specified, we simply return "Alabama"

	PARAMETERS:
		state: the user inputted string representing the state they want to query over

	RETURN:
		a String representing the user's inputted state, but with the first letter of
		each word capitalized (except for prepositions and conjunctions) or Alabama if
		no String was entered.
	'''
	state = state.strip()

	if state == "":
		state = "Alabama"

	correctedState = ""
	wordList = state.split(" ")

	for word in wordList:
		correctedWord = cleanIndividualWord(word)
		correctedState = correctedState + correctedWord + " "

	correctedState = correctedState.strip()

	return correctedState


def cleanIndividualWord(word):
	'''
	Returns the inputted word with the first letter capitalized unless
	the inputted word is a preposition or conjunction.

	PARAMETERS:
		word- the word to capitalize (or not capitalize)

	RETURN:
		a String representing the word, but capitalized if necessary
	'''
	nonCapitalizedWords = ["a", "an", "for", "and", "or", "nor", "but", "yet", "so", "at",
	 "around", "by", "after", "along", "from", "of", "on", "to", "with", "without"]
	word = word.lower()
	if word not in nonCapitalizedWords:
		word = word[0].capitalize() + word[1:]

	return word


def getNationalQueryData(startYear, endYear):
	'''
	Returns the average annual rate of homicide per 100,000 people across the nation over the specified
	year range, the state/region with the highest average annual rate of homicide over this range and
	its rate of homicide, a list containing each year within the specified range (for our Javascript files),
	and the rate of homicide in each individual year in the specified range stored in a list.

	PARAMETERS:
		startYear- the first year to collect national data for
		endYear- the last year over which we will collect data

	RETURN:
		A dictionary with keys: "nationalCrudeRate" - the national rate of homicide per 100,000 people
		over the specified years, "mostDangerousState" - the state with the highest homicide rate,
		"mostDangerousStateRate" - the rate of homicide of the most dangerous state,
		"yearRange" - a list of years, beginning with the start year and ending with the end year, and
		"singleYearCrudeRates" - the national rate of homicide each year in the range, stored in a list
	'''
	nationalQueryData = {}
	nationTotals = dataSource.getUSATotals(startYear, endYear)
	if isinstance(nationTotals, Exception):
		raise nationTotals

	nationalQueryData["nationalCrudeRate"] = getNationalCrudeRate(nationTotals)
	nationalQueryData["mostDangerousState"], nationalQueryData["mostDangerousStateRate"] = getMostDangerousStateAndData(startYear, endYear)
	nationalQueryData["yearRange"] = getYearRange(startYear, endYear)
	nationalQueryData["singleYearCrudeRates"] = getNationalSingleYearCrudeRates(startYear, endYear)

	return nationalQueryData


def getMostDangerousStateAndData(startYear, endYear):
	'''
	Returns the US state with the highest rate of homicide over the specified
	range of years and the rate of homicide within this state per 100,000 people in the
	population

	PARAMETERS:
		startYear - the first year over which to find homicide data
		endYear- the last year to collect homicide data over

	RETURN:
		The most dangerous state, returned as a string, and the state's
		average annual rate of homicide over the specified range (per 100,000 people)
		returned as an int
	'''
	crudeRate = 0
	currentStateRate = 0
	mostDangerousState = ""

	for state in dataSource.stateDictionary:
		currentStateRate = getStateCrudeRate(dataSource.getStateQuery(startYear, endYear, state))

		if (currentStateRate > crudeRate):
			crudeRate = currentStateRate
			mostDangerousState = state

	return mostDangerousState, crudeRate


def getNationalSingleYearCrudeRates(startYear, endYear):
	'''
	Gets the national rate of homicide over each year from startYear to endYear, places all of these
	crude rates into a list of ints and returns this list

    PARAMETERS:
    startYear: the first year to find the homicide crude rate for
    endYear: the last year to find the homicide crude rate for

    RETURN:
    A list of ints each representing the national rate of homicide per 100,000 people in
	each year within the specified range

    Calls getNationalCrudeRate
    '''
	list = []
	rate = 0
	crudeRates = []

	for year in range (startYear, endYear + 1):
		list = dataSource.getUSATotals(year, year)
		rate = getNationalCrudeRate(list)
		crudeRates.append(rate)

	return crudeRates


@app.route('/', methods = ['POST', 'GET'])
def getNationalQueryResults():
	'''
	Loads the homepage and returns a results page corresponding to the user's query. Directs
	user to an error page if the query was not formatted properly
	'''
	try:
		start = request.args.get('startYear')
		end = request.args.get('endYear')
		start, end = adjustYears(start, end)
		start, end = setYearsToInts(start, end)

		dataTable = getNationalQueryData(start, end)
		
		return render_template('HomePage2.html',
									inputdata = dataTable["singleYearCrudeRates"],
									inputlabels = dataTable["yearRange"],
									inputtitle = f"National Homicide Rate from {{start}} to {{end}}",
									nationalCrudeRate = dataTable["nationalCrudeRate"],
									startYear = start,
									endYear = end,
									mostDangerousState = dataTable["mostDangerousState"],
									mostDangerousStateRate = dataTable["mostDangerousStateRate"])

	except Exception as e:

		return render_template('Error.html', error = e)


@app.route('/stateQuery/')
def getMapQueryResults():
	'''
	Loads a resulting state query page if the user clicks on one of the states in the
	interactive map or otherwise queries for a state
	'''
	if(request.method == 'GET'):

		try:
			start = request.args.get('startYear')
			end = request.args.get('endYear')
			start, end = adjustYears(start, end)
			start, end = setYearsToInts(start, end)
			state = request.args.get('state')
			state = cleanStateInput(state)
			
			dataTable = getStateQueryData(start, end, state)
			
			return render_template('Results.html', stateCrudeRate = dataTable["stateCrudeRate"],
										nationalCrudeRate = dataTable["nationalCrudeRate"],
										causesAndPercentages = dataTable["causesAndPercentages"],
										state = state,
										startYear = start,
										endYear = end,
										inputdata = dataTable["singleYearCrudeRates"],
										inputlabels = dataTable["yearRange"],
										inputtitle = f"{state} Annual Crude Rates",
										inputpiedata= list(dataTable["causesAndPercentages"].values()),
										inputpielabels= list(dataTable["causesAndPercentages"].keys()),
										inputpietitle=f"{state} Homicide Data by Cause of Death")

		except Exception as e:

			return render_template('Error.html', error = e)

	else:

		state = Alabama
		start = 1999
		end = 2017
		dataTable = getStateQueryData(start, end, state)

		return render_template('Results.html', stateCrudeRate = dataTable["stateCrudeRate"],
										nationalCrudeRate = dataTable["nationalCrudeRate"],
										causesAndPercentages = dataTable["causesAndPercentages"],
										state = state,
										startYear = start,
										endYear = end)


if __name__ == '__main__':
	if len(sys.argv) != 3:
		print('Usage: {0} host port'.format(sys.argv[0]), file=sys.stderr)
		exit()

	host = sys.argv[1]
	port = sys.argv[2]
	app.run(host=host, port=port)
