PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS
Articles(
    Name TEXT NOT NULL PRIMARY KEY,
    sellPrice NUMERIC NOT NULL,
    jShare NUMERIC NOT NULL,
    bPrice NUMERIC NOT NULL,
    reducedPrice NUMERIC NOT NULL,
    function INTEGER DEFAULT 0 REFERENCES Functions(Id) ON UPDATE CASCADE ON DELETE SET DEFAULT);

CREATE TABLE IF NOT EXISTS
Functions(
    Name TEXT NOT NULL UNIQUE,
    Id INTEGER NOT NULL UNIQUE,
    PRIMARY KEY(Name, Id));

CREATE TABLE IF NOT EXISTS
Clients(
    Name TEXT NOT NULL PRIMARY KEY,
    phone TEXT,
    address TEXT,
    email TEXT,
    negLimit NUMERIC,
    isJobist INTEGER,
    balance NUMERIC NOT NULL);

CREATE TABLE IF NOT EXISTS
SaleSessions(
    OpeningTime DATE NOT NULL PRIMARY KEY,
    closingTime DATE,
    openAmount NUMERIC,
    closeAmount NUMERIC,
    soldAmount NUMERIC); --To be stored in case prices changed after the sale

CREATE TABLE IF NOT EXISTS
HeldSession(
    Name TEXT NOT NULL REFERENCES Clients(Name) ON DELETE CASCADE ON UPDATE CASCADE,
    SessionTime DATE NOT NULL REFERENCES SaleSessions(OpeningTime) ON DELETE CASCADE ON UPDATE CASCADE,
    PRIMARY KEY(Name, SessionTime));

CREATE TABLE IF NOT EXISTS
FunctionBenefits(
    FctId INTEGER NOT NULL DEFAULT 0 REFERENCES Functions(Id) ON DELETE SET DEFAULT ON UPDATE CASCADE,
    SessionTime DATE NOT NULL REFERENCES SaleSessions(OpeningTime) ON DELETE NO ACTION ON UPDATE CASCADE,
    amount NUMERIC,
    PRIMARY KEY(FctId, SessionTime));

CREATE TABLE IF NOT EXISTS
OrderDetails(
    OrderId INTEGER PRIMARY KEY,
    sessionTime DATE NOT NULL REFERENCES SaleSessions(OpeningTime) ON DELETE CASCADE,
    orderNumber INTEGER NOT NULL,
    UNIQUE(sessionTime, orderNumber));

CREATE TABLE IF NOT EXISTS
OrderContent(
    OrderId INTEGER NOT NULL REFERENCES OrderDetails(OrderId) ON DELETE CASCADE,
    Article TEXT NOT NULL REFERENCES Articles(Name) ON DELETE NO ACTION,
    amount INTEGER NOT NULL CHECK(amount > 0),
    PRIMARY KEY(OrderId, Article));

CREATE TABLE IF NOT EXISTS
OrderClient(
    OrderId INTEGER NOT NULL REFERENCES OrderDetails(OrderId) ON DELETE CASCADE,
    Client TEXT NOT NULL REFERENCES Clients(Name) ON DELETE NO ACTION,
    PRIMARY KEY(OrderId, Client));

INSERT OR UPDATE INTO Functions VALUES('Unknown Function', 0);



-- For the maybe to come history database
CREATE TABLE IF NOT EXISTS
SaleSessions(
    OpeningTime DATE NOT NULL PRIMARY KEY,
    closingTime DATE,
    jShare NUMERIC NOT NULL, --not in day to day database
    openAmount NUMERIC,
    closeAmount NUMERIC,
    soldAmount NUMERIC);
