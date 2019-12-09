import psycopg2
import getpass

class DataSource:
	'''
	DataSource executes all of the queries on the database.
	It also formats the data to send back to the frontend, typically in a list
	or some other collection or object.
	'''

	def __init__(self, connection):
		self.connection = connection
		self.stateDictionary = {
				"Alabama" : "AL",
				"Alaska" : "AK",
				"Arizona" : "AZ",
				"Arkansas" : "AR",
				"California" : "CA",
				"Colorado" : "CO",
				"Connecticut" : "CT",
				"Delaware" : "DE",
				"Florida" : "FL",
				"Georgia" : "GA",
				"Hawaii" : "HI",
				"Idaho" : "ID",
				"Illinois" : "IL",
				"Indiana" : "IN",
				"Iowa" : "IA",
				"Kansas" : "KS",
				"Kentucky" : "KY",
				"Louisiana" : "LA",
				"Maine" : "ME",
				"Maryland" : "MD",
				"Massachusetts" : "MA",
				"Michigan" : "MI",
				"Minnesota" : "MN",
				"Mississippi" : "MS",
				"Missouri" : "MO",
				"Montana" : "MT",
				"Nebraska" : "NE",
				"Nevada" : "NV",
				"New Hampshire" : "NH",
				"New Jersey" : "NJ",
				"New Mexico" : "NM",
				"New York" : "NY",
				"North Carolina" : "NC",
				"North Dakota" : "ND",
				"Ohio" : "OH",
				"Oklahoma" : "OK",
				"Oregon" : "OR",
				"Pennsylvania" : "PA",
				"Rhode Island" : "RI",
				"South Carolina" : "SC",
				"South Dakota" : "SD",
				"Tennessee" : "TN",
				"Texas" : "TX",
				"Utah" : "UT",
				"Vermont" : "VT",
				"Virginia" : "VA",
				"Washington" : "WA",
				"West Virginia" : "WV",
				"Wisconsin" : "WI",
				"Wyoming" : "WY",
				"District of Columbia" : "DC"
				}


	def getUSAQuery(self, startYear, endYear):
		'''
		returns a list of all states and their associated homicide data

		PARAMETERS:
			startYear - the first year of data to draw from
			endYear - the last year of data to draw from

		RETURN:
			a list of all of the states and associated homicide data for each
			year

		Calls USASingleYearQuery
		'''
		results = []

		try:
			self.checkValidRange(startYear, endYear)

		except Exception as e:
			return e

		yearRange = endYear - startYear + 1

		try:
			for i in range(yearRange):
				currentYear = i + startYear
				results.append(self.getUSASingleYearQuery(currentYear))

			return results

		except Exception as e:
			print("Something went wrong when executing the query(USA): " + str(e))
			return None

		return []


	def getUSASingleYearQuery(self, year):
		'''
		returns a list of all states and thier associated homicide data
		
		PARAMETERS:
			year: the year of data to draw from
		
		RETURN:
			a list of all states and associated data
		'''

		try:
			self.checkValidYear(year)

		except Exception as e:
			return e

		results = []

		try:
			cursor = self.connection.cursor()
			query = f"SELECT * FROM states{year}"
			cursor.execute(query)
			results = cursor.fetchall()

		except Exception as e:
			print("Something when wrong when excecuting the query (state)" + str(e))

		return results

		return []


	def getUSATotals(self, startYear, endYear):
		'''
		returns just the totals for the whole USA and each state

		PARAMETERS:
			startYear: The first year to gather data for
			endYear: the lastyear to gather data for (inclusive)

		calls getUSASingleYearTotals
		'''
		try:
			self.checkValidRange(startYear, endYear)

		except Exception as e:
			return e
		
		results = []
		yearRange = endYear - startYear + 1

		for i in range(yearRange):
			currentYear = startYear + i
			results.append(self.getUSASingleYearTotals(currentYear))

		return results


	def getUSASingleYearTotals(self, year):
		'''
		returns the totals for the whole USA and each state

		PARAMETERS:
			year: the year to gather data for
		'''
		results = []

		try:
			self.checkValidYear(year)

		except Exception as e:
			return e

		try:
			cursor = self.connection.cursor()
			query = f"SELECT * FROM states{year} WHERE notes = 'Total'"
			cursor.execute(query)
			results = cursor.fetchall()

		except Exception as e:
			print("Something went wrong when executing the query (USATotals)" + str(e))

		return results


	def getStateQuery(self, startYear, endYear, state):
		'''
		returns a list of data for the specified state, including both general
		data and data for each county

		PARAMETERS:
			startYear - the first year of data to draw from
			endYear - the last year of data to draw from
			state: the state to get data for

		RETURN:
			a list of data for the state, as a list of lists

		Calls StateSingleYearQuery
		'''

		try:
			self.checkValidRange(startYear, endYear)
			self.checkValidState(state)

		except Exception as e:
			return e

		results = []
		yearDifference = endYear - startYear
		i = 0

		while i <= yearDifference:
			results.append(self.getStateSingleYearQuery(startYear + i, state))
			i = i + 1

		return results


	def getStateSingleYearQuery(self, year, state):
		'''
		returns a list of data for the specified state, including both general
		data and data for each county, for a single year

		PARAMETERS:
			year: the year to get data for
			state: the state to get data for

		RETURN:
			a list of data for the state, as a list of lists.
		'''

		try:
			self.checkValidYear(year)
			self.checkValidState(state)

		except Exception as e:
			return e

		results = []

		try:
			cursor = self.connection.cursor()
			query = f"SELECT * FROM states{year} WHERE statename = '{state}'"
			cursor.execute(query)
			results = cursor.fetchall()

		except Exception as e:
			print("Something when wrong when excecuting the query (state)")
		
		countyPattern = self.getCountyPatternForState(state)
		countyData = self.getCountySingleYearQuery(year, countyPattern)
		results.append(countyData)

		return results


	def getCountyPatternForState(self, state):
		'''
		returns an pattern that will match for every county in the state

		PARAMETERS:
			state: the state to find the counties of

		RETURN:
			a pattern that will match all counties of the state with a LIKE operator
		'''

		return f"%{self.stateDictionary.get(state)}"


	def getCountyQuery(self,  startYear, endYear, county):
		'''
		returns a list of data for a specific county or list of counties (using LIKE)

		PARAMETERS:
			startYear - an integer, the first year to get data for
			endYear - an integer, the last year to get data for
			county - the expression defining which county names may be excepted
		RETURN:
			a list of data for the county

		'''

		results = []

		try:
			self.checkValidRange(startYear, endYear)
			self.checkValidCounty(county)

		except Exception as e:
			return None

		yearRange = endYear - startYear + 1

		try:
			for i in range(yearRange):
				currentYear = i + startYear
				results.append(self.getCountySingleYearQuery(currentYear, county))

			return results

		except Exception as e:
			print("Something went wrong when executing the query(county): " + str(e))
			return None


	def getCountySingleYearQuery(self, year, county):
		'''
		returns county data for a single year for one county or a list
		of counties (using LIKE)

		PARAMETERS:
			year - the year to get data for
			county -  the expression defining which county names may be excepted
		'''

		try:
			self.checkValidYear(year)
			self.checkValidCounty(county)

		except Exception as e:
			return None 

		results = []

		cursor = self.connection.cursor()
		query = f"SELECT * FROM counties{year} WHERE county LIKE '{county}'"
		cursor.execute(query)
		results = cursor.fetchall()

		return results


	def checkValidState(self, state):
		'''
		Returns true if the state is a valid US State name. Throws a TypeError
		if state is not a String and ValueError if it is not a US State.

		PARAMETERS:
			state: the string to check for validity
		'''

		if not isinstance(state, str):
			print("State must be a string")
			raise TypeError("State must be a string")

		if not state in self.stateDictionary:
			print("State not found")
			raise ValueError("State not found")

		return True


	def checkValidYear(self, year):
		'''
		Returns true if the year is within range 1999 to 2017. Raises TypeError if
		argument is not an int. Raises ValueError if int is too large or small.

		PARAMETERS:
			year: the int to check for validity
		'''

		if not isinstance(year, int):
			print("Year must be an integer")
			raise TypeError("Year must be an integer")

		if(year < 1999 or year > 2017):
			print("Invalid year")
			raise ValueError("Invalid year")

		return True


	def checkValidRange(self, startYear, endYear):
		'''
		Returns true if the year range is valid and within 1999 to 2017. Raises TypeError if either
		argument is not an int. Raises ValueError if the start year is greater than end year
		or the range is not within 1999-2017.
		'''

		if not (isinstance(startYear, int) and isinstance(endYear, int)):
			raise TypeError("Years must be integers")

		if (startYear < 1999 or endYear > 2017 or startYear > endYear):
			raise ValueError("Invalid year range")

		return True


	def checkValidCounty(self, county):
		'''
		Returns true if the county is a string, and raises a TypeError otherwise
		'''

		if not isinstance(county, str):
			print("county must be a year")
			raise TypeError

		return True


	def disconnect():
		self.connection.close()


def connect(user, password):
	'''
	Establishes a connection to the database with the following credentials:
	user - username, which is also the name of the database
	password - the password for this database on perlman

	Returns: a database connection.

	Note: exits if a connection cannot be established.
	'''

	try:
		connection = psycopg2.connect(database=user, user=user, password=password)

	except Exception as e:
		print("Connection error: ", e)
		exit()

	return connection
