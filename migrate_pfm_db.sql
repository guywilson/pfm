ALTER TABLE recurring_charge
ADD COLUMN last_payment_date TEXT;

DROP VIEW v_recurring_charge_list;

CREATE VIEW v_recurring_charge_list AS 
SELECT rc.id,rc.account_id,rc.date,rc.end_date,rc.last_payment_date, rc.description,rc.payee_id,p.code AS payee_code,rc.category_id,c.code AS category_code,rc.amount,rc.frequency,rc.created,rc.updated 
FROM recurring_charge rc 
LEFT JOIN payee p ON p.id = rc.payee_id 
LEFT JOIN category c ON c.id = rc.category_id 
ORDER BY rc.last_payment_date ASC;

CREATE TABLE recurring_transfer (
id INTEGER PRIMARY KEY,
recurring_charge_id INTEGER,
account_to_id INTEGER,
created TEXT NOT NULL,
updated TEXT NOT NULL,
FOREIGN KEY(account_to_id) REFERENCES account(id),
FOREIGN KEY(recurring_charge_id) REFERENCES category(id));

UPDATE account_transaction
SET category_id = NULL
WHERE category_id = 0;

UPDATE account_transaction
SET payee_id = NULL
WHERE payee_id = 0;

UPDATE account_transaction
SET recurring_charge_id = NULL
WHERE recurring_charge_id = 0;
