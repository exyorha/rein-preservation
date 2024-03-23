-- Update the constraints on i_user_costume_awaken_status_up

CREATE TABLE new_i_user_costume_awaken_status_up (
  user_id INTEGER NOT NULL,
  user_costume_uuid text NOT NULL,
  status_calculation_type integer NOT NULL,
  hp integer NOT NULL DEFAULT 0,
  attack integer NOT NULL DEFAULT 0,
  vitality integer NOT NULL DEFAULT 0,
  agility integer NOT NULL DEFAULT 0,
  critical_ratio integer NOT NULL DEFAULT 0,
  critical_attack integer NOT NULL DEFAULT 0,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, user_costume_uuid, status_calculation_type)
);

INSERT INTO new_i_user_costume_awaken_status_up
    SELECT user_id, user_costume_uuid, status_calculation_type, hp, attack, vitality, agility, critical_ratio, critical_attack, latest_version FROM i_user_costume_awaken_status_up;

DROP TRIGGER i_user_costume_awaken_status_up_update_version;
DROP INDEX i_user_costume_awaken_status_up_user_id;
DROP TABLE i_user_costume_awaken_status_up;

ALTER TABLE new_i_user_costume_awaken_status_up RENAME TO i_user_costume_awaken_status_up;

CREATE INDEX i_user_costume_awaken_status_up_user_id ON i_user_costume_awaken_status_up(user_id);

CREATE TRIGGER i_user_costume_awaken_status_up_update_version
AFTER UPDATE ON i_user_costume_awaken_status_up FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_costume_awaken_status_up SET latest_version = OLD.latest_version + 1 WHERE i_user_costume_awaken_status_up.rowid = NEW.rowid;
END;
