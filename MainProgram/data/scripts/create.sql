--Notes file is parsed by the program.
--Notes:
--File does not support multiline comments.
--Each statement has to start with a comment line containing 'Statement' with no space and with that exact CaSe.

--Statement
--1
PRAGMA foreign_keys = ON;

--Statement
--2
CREATE TABLE IF NOT EXISTS
Articles(
    Name TEXT NOT NULL PRIMARY KEY,
    sellPrice NUMERIC NOT NULL,
    jShare NUMERIC NOT NULL,
    bPrice NUMERIC NOT NULL,
    reducedPrice NUMERIC NOT NULL,
    function INTEGER DEFAULT 0 REFERENCES Functions(Id) ON UPDATE CASCADE ON DELETE SET DEFAULT);

--Statement
--3
CREATE TABLE IF NOT EXISTS
Functions(
    name TEXT NOT NULL UNIQUE COLLATE NOCASE, --Functions with the same name but different case will be considered identical
    Id INTEGER PRIMARY KEY);

--Statement
--4
CREATE TABLE IF NOT EXISTS
Clients(
    Name TEXT NOT NULL PRIMARY KEY,
    phone TEXT DEFAULT '',
    address TEXT DEFAULT '',
    email TEXT DEFAULT '',
    negLimit NUMERIC DEFAULT 0,
    isJobist INTEGER DEFAULT 0,
    balance NUMERIC NOT NULL DEFAULT 0 CHECK(balance >= negLimit),
    CHECK( (isJobist = 1) OR (isJobist = 0 AND negLimit = 0)));

--Statement
--5
CREATE TABLE IF NOT EXISTS
SaleSessions(
    OpeningTime DATE NOT NULL PRIMARY KEY,
    closingTime DATE,
    state TEXT NOT NULL DEFAULT 'opened' CHECK(state IN('opened', 'closed')),
    openAmount NUMERIC,
    closeAmount NUMERIC,
    CHECK (((closingTime IS NULL) AND state = 'opened') OR (closingTime IS NOT NULL AND closingTime > OpeningTime AND state = 'closed')));

--Statement
--6
CREATE TABLE IF NOT EXISTS
HeldSession(
    Name TEXT NOT NULL REFERENCES Clients(Name) ON DELETE CASCADE ON UPDATE CASCADE,
    SessionTime DATE NOT NULL REFERENCES SaleSessions(OpeningTime) ON DELETE CASCADE ON UPDATE CASCADE,
    PRIMARY KEY(Name, SessionTime));

--Statement
--7
CREATE TABLE IF NOT EXISTS
Transactions(
    Id INTEGER PRIMARY KEY,
    sessionTime DATE NOT NULL REFERENCES SaleSessions(OpeningTime) ON DELETE CASCADE,
    lineNumber INT NOT NULL,
    processTime DATE NOT NULL,
    total REAL NOT NULL,
    UNIQUE(sessionTime, lineNumber));

--Statement
--8
CREATE TABLE IF NOT EXISTS
IsOrder(
    Id INTEGER PRIMARY KEY REFERENCES Transactions(Id) ON DELETE CASCADE,
    price TEXT NOT NULL,
    CHECK (price IN('normal', 'reduced', 'free')));

--Statement
--9
CREATE TABLE IF NOT EXISTS
OrderContent(
    Id INTEGER REFERENCES IsOrder(Id) ON DELETE CASCADE,
    article TEXT NOT NULL REFERENCES Articles(name) ON DELETE NO ACTION,
    quantity INTEGER NOT NULL CHECK (quantity > 0),
    PRIMARY KEY(Id, article));

--Statement
--10
CREATE TABLE IF NOT EXISTS
OrderClient(
    Id INTEGER PRIMARY KEY REFERENCES IsOrder(Id) ON DELETE CASCADE,
    client TEXT NOT NULL REFERENCES Clients(Name) ON DELETE NO ACTION);

--Statement
--11
CREATE TABLE IF NOT EXISTS
CashMoves(
    Id INTEGER PRIMARY KEY REFERENCES Transactions(Id) ON DELETE CASCADE,
    client TEXT,
    note TEXT);

--Statement
--12
CREATE TABLE IF NOT EXISTS
Config(
    Field TEXT NOT NULL PRIMARY KEY,
    value TEXT);

--Statement
--13
CREATE TRIGGER IF NOT EXISTS order_check
BEFORE INSERT ON IsOrder
WHEN NEW.Id IN (SELECT Id FROM CashMoves)
BEGIN
    SELECT Raise(FAIL, 'Line number is already a cash move (withdraw/deposit).');
END;

--Statement
--14
CREATE TRIGGER IF NOT EXISTS cashMove_check
BEFORE INSERT ON CashMoves
WHEN NEW.Id IN (SELECT Id FROM IsOrder)
BEGIN
    SELECT Raise(FAIL, 'Line number is already an order.');
END;

--Statement
--15
CREATE TRIGGER IF NOT EXISTS transaction_time
BEFORE INSERT ON Transactions
WHEN NEW.processTime < NEW.sessionTime
BEGIN
SELECT Raise(Fail, 'Added Transaction happened before the begining of the session.');
END;

--Statement
--16
CREATE TRIGGER IF NOT EXISTS transaction_session_state
BEFORE INSERT ON Transactions
WHEN (SELECT state FROM SaleSessions WHERE SaleSessions.OpeningTime = NEW.sessionTime) = 'closed'
BEGIN
SELECT Raise(Fail, 'Cannot add a transaction to a closed sale session.');
END;

--Statement
--17
CREATE TRIGGER IF NOT EXISTS CurrentSession_exists
BEFORE UPDATE OF value ON Config
WHEN (NEW.Field='CurrentSession' AND ((NEW.value NOT IN (SELECT OpeningTime FROM SaleSessions)) AND (New.value NOT NULL)))
BEGIN
SELECT Raise(Fail, 'Current Session must be an existing session or null.');
END;

--Statement
--18
CREATE TRIGGER IF NOT EXISTS CurrentSessionOrderId_valid
BEFORE UPDATE OF value ON Config
WHEN (NEW.Field='CurrentSessionOrderId' AND (NEW.value < 0 OR NEW.value IS NULL))
BEGIN
SELECT Raise(Fail, 'Current Session Order Id must be a positive Integer.');
END;

--Statement
--19
CREATE TRIGGER IF NOT EXISTS CashMove_has_valid_client
BEFORE UPDATE ON CashMoves
WHEN (NEW.client NOT NULL AND NEW.client NOT IN (SELECT name FROM Clients))
BEGIN
SELECT Raise(Fail, 'Current Session Order Id must be a positive Integer.');
END;

--21 to 29
--Statement
INSERT OR REPLACE INTO Functions VALUES('No Function', 0);

--Statement
INSERT OR IGNORE INTO Config VALUES('CurrentSession', NULL);

--Statement
INSERT OR IGNORE INTO Config VALUES('CurrentSessionOrderId', 1);

--Statement
INSERT OR IGNORE INTO Config VALUES('PinHash', ''); --TODO Edit for real default hash

--Statement
INSERT OR IGNORE INTO Config VALUES('FirstStart', 1);

--Statement
INSERT OR IGNORE INTO Config VALUES('ManagerHash', ''); --TODO Edit for real default hash

--Statement
INSERT OR IGNORE INTO Config VALUES('TreasurerHash', ''); --TODO Edit for real default hash

--Statement
INSERT OR IGNORE INTO Config VALUES('MinJShare', 10);

--Statement
INSERT OR IGNORE INTO Config VALUES('MinNegLimit', -10);

--Statement
--20
CREATE TRIGGER IF NOT EXISTS Client_has_valid_negLimit
AFTER UPDATE OF negLimit ON Clients
WHEN (NEW.negLimit < CAST( (SELECT value FROM Config WHERE Field='MinNegLimit') AS REAL))
BEGIN
UPDATE Clients SET NegLimit = (SELECT value FROM Config WHERE field='MinNegLimit')
WHERE Name = NEW.Name;
END;
