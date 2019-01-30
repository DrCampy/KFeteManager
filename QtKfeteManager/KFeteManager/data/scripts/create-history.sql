--statement
CREATE TABLE IF NOT EXISTS
Clients(
    Name TEXT PRIMARY KEY);

--statement
CREATE TABLE IF NOT EXISTS
Articles(
    Name TEXT PRIMARY KEY);

--statement
CREATE TABLE IF NOT EXISTS
Sessions(
    OpenTime INTEGER PRIMARY KEY,
    closeTime INTEGER NOT NULL,
    openAmount NUMERIC,
    closeAmount NUMERIC,
    incomes NUMERIC,
    jobistShare NUMERIC,
    jobistWage NUMERIC);

--statement
CREATE TABLE IF NOT EXISTS
Jobists(
    Session INT REFERENCES Sessions(OpenTime) ON DELETE CASCADE,
    Client TEXT DEFAULT '****' REFERENCES Clients(Name) ON DELETE SET DEFAULT,
    PRIMARY KEY(Session, Client));

--statement
CREATE TABLE IF NOT EXISTS
FunctionsBenefits(
    Session INT NOT NULL REFERENCES Sessions(OpenTime) ON DELETE CASCADE,
    Function TEXT NOT NULL,
    Amount NUMERIC NOT NULL,
    PRIMARY KEY(Session, Function));

--statement
CREATE TABLE IF NOT EXISTS
AccountMoves(
    Session INT REFERENCES Sessions(OpenTime) ON DELETE CASCADE,
    Client TEXT REFERENCES Clients(Name),
    amount NUMERIC NOT NULL,
    note TEXT);

--statement
CREATE TABLE IF NOT EXISTS
CashMoves(
    Session INT REFERENCES Sessions(OpenTime) ON DELETE CASCADE,
    Amount NUMERIC NOT NULL,
    note TEXT);

--statement
CREATE TABLE IF NOT EXISTS
Sales(
    Session INT REFERENCES Sessions(OpenTime) ON DELETE CASCADE,
    Article TEXT NOT NULL,
    quantity INTEGER NOT NULL CHECK (quantity > 0),
    price TEXT NOT NULL CHECK(price IN('normal', 'reduced', 'free')),
    PRIMARY KEY(Session, Article, price));

--statement
CREATE TABLE IF NOT EXISTS
ClientHistory(
    Client TEXT REFERENCES Clients(Name),
    Article TEXT REFERENCES Articles(Name),
    quantity INTEGER DEFAULT 0 CHECK(quantity > 0),
    PRIMARY KEY(Client, Article));

--Collate article names
