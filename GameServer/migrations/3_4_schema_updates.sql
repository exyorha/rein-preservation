-- Update the constraints on i_user_side_story_quest

CREATE TABLE new_i_user_side_story_quest (
  user_id INTEGER NOT NULL,
  side_story_quest_id integer NOT NULL,
  head_side_story_quest_scene_id integer NOT NULL,
  side_story_quest_state_type integer NOT NULL,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, side_story_quest_id)
);

INSERT INTO new_i_user_side_story_quest
    SELECT user_id, side_story_quest_id, head_side_story_quest_scene_id, side_story_quest_state_type, latest_version
    FROM i_user_side_story_quest;

DROP TRIGGER i_user_side_story_quest_update_version;
DROP INDEX i_user_side_story_quest_user_id;
DROP TABLE i_user_side_story_quest;

ALTER TABLE new_i_user_side_story_quest RENAME TO i_user_side_story_quest;

-- Update the constraints on i_user_side_story_quest_scene_progress_status

CREATE TABLE new_i_user_side_story_quest_scene_progress_status (
  user_id INTEGER NOT NULL PRIMARY KEY,
  current_side_story_quest_id integer NOT NULL,
  current_side_story_quest_scene_id integer NOT NULL,
  latest_version bigint NOT NULL DEFAULT 1
);

INSERT INTO new_i_user_side_story_quest_scene_progress_status
    SELECT user_id, current_side_story_quest_id, current_side_story_quest_scene_id, latest_version
    FROM i_user_side_story_quest_scene_progress_status;

DROP TRIGGER i_user_side_story_quest_scene_progress_status_update_version;

DROP TABLE i_user_side_story_quest_scene_progress_status;

ALTER TABLE new_i_user_side_story_quest_scene_progress_status RENAME TO i_user_side_story_quest_scene_progress_status;

CREATE TRIGGER i_user_side_story_quest_update_version
AFTER UPDATE ON i_user_side_story_quest FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_side_story_quest SET latest_version = OLD.latest_version + 1
    WHERE i_user_side_story_quest.rowid = NEW.rowid;
END;

-- Update the constraints on i_user_explore
CREATE TABLE new_i_user_explore (
  user_id INTEGER NOT NULL PRIMARY KEY,
  is_use_explore_ticket boolean NOT NULL DEFAULT 0,
  playing_explore_id integer NOT NULL DEFAULT 0,
  latest_play_datetime timestamp NOT NULL DEFAULT 0,
  latest_version bigint NOT NULL DEFAULT 1
);

INSERT INTO new_i_user_explore
    SELECT user_id, is_use_explore_ticket, playing_explore_id, latest_play_datetime, latest_version FROM i_user_explore;

DROP TRIGGER i_user_explore_update_version;
DROP TABLE i_user_explore;

ALTER TABLE new_i_user_explore RENAME TO i_user_explore;

CREATE TRIGGER i_user_explore_update_version
AFTER UPDATE ON i_user_explore FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_explore SET latest_version = OLD.latest_version + 1
    WHERE i_user_explore.rowid = NEW.rowid;
END;

-- Update the constraints i_user_explore_score
CREATE TABLE new_i_user_explore_score (
  user_id INTEGER NOT NULL,
  explore_id integer NOT NULL,
  max_score integer NOT NULL DEFAULT 0,
  max_score_update_datetime timestamp NOT NULL DEFAULT 0,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, explore_id)
);

INSERT INTO new_i_user_explore_score
  SELECT user_id, explore_id, max_score, max_score_update_datetime, latest_version FROM i_user_explore_score;

DROP TRIGGER i_user_explore_score_update_version;
DROP INDEX i_user_explore_score_user_id;
DROP TABLE i_user_explore_score;

ALTER TABLE new_i_user_explore_score RENAME TO i_user_explore_score;

CREATE INDEX i_user_explore_score_user_id ON i_user_explore_score(user_id);

CREATE TRIGGER i_user_explore_score_update_version
AFTER UPDATE ON i_user_explore_score FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_explore_score SET latest_version = OLD.latest_version + 1
    WHERE i_user_explore_score.rowid = NEW.rowid;
END;

-- Add the tables for the gift system
CREATE TABLE internal_user_gift (
  gift_id INTEGER PRIMARY KEY NOT NULL, -- serves as 'user_gift_uuid', but is easier to work with
  user_id INTEGER NOT NULL,
  grant_datetime INTEGER NOT NULL,
  expires_datetime INTEGER NOT NULL,
  received_datetime INTEGER NOT NULL DEFAULT 0, -- 0 means 'not yet received'
  gift_data BLOB NOT NULL, -- this is serialized apb.api.gift.GiftCommon
  reward_kind_type INTEGER NOT NULL
);

CREATE INDEX internal_user_gift_user_id ON internal_user_gift (user_id);

-- Update the constraints on i_user_premium_item
CREATE TABLE new_i_user_premium_item (
  user_id INTEGER NOT NULL,
  premium_item_id integer NOT NULL,
  acquisition_datetime timestamp NOT NULL,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, premium_item_id)
);

INSERT INTO new_i_user_premium_item
  SELECT user_id, premium_item_id, acquisition_datetime, latest_version FROM i_user_premium_item;

DROP TRIGGER i_user_premium_item_update_version;
DROP INDEX i_user_premium_item_user_id;
DROP TABLE i_user_premium_item;

ALTER TABLE new_i_user_premium_item RENAME TO i_user_premium_item;

CREATE INDEX i_user_premium_item_user_id ON i_user_premium_item(user_id);

CREATE TRIGGER i_user_premium_item_update_version
AFTER UPDATE ON i_user_premium_item FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_premium_item SET latest_version = OLD.latest_version + 1
    WHERE i_user_premium_item.rowid = NEW.rowid;
END;

-- Update the constraints on i_user_parts_group_note
CREATE TABLE new_i_user_parts_group_note (
  user_id INTEGER NOT NULL,
  parts_group_id integer NOT NULL,
  first_acquisition_datetime timestamp NOT NULL,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, parts_group_id)
);

INSERT INTO new_i_user_parts_group_note
  SELECT user_id, parts_group_id, first_acquisition_datetime, latest_version FROM i_user_parts_group_note;

DROP INDEX i_user_parts_group_note_user_id;
DROP TRIGGER i_user_parts_group_note_update_version;
DROP TABLE i_user_parts_group_note;

ALTER TABLE new_i_user_parts_group_note RENAME TO i_user_parts_group_note;

CREATE INDEX i_user_parts_group_note_user_id ON i_user_parts_group_note(user_id);

CREATE TRIGGER i_user_parts_group_note_update_version
AFTER UPDATE ON i_user_parts_group_note FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_parts_group_note SET latest_version = OLD.latest_version + 1
    WHERE i_user_parts_group_note.rowid = NEW.rowid;
END;


SELECT DISTINCT user_id, parts_group_id FROM i_user_parts, m_parts USING (parts_id);

-- Initially populate i_user_parts_group_note, it used to be unpopulated
INSERT INTO
  i_user_parts_group_note
  (user_id, parts_group_id, first_acquisition_datetime)
  SELECT
    user_id,
    parts_group_id,
    MIN(acquisition_datetime)
  FROM
    i_user_parts,
    m_parts USING (parts_id)
    LEFT JOIN i_user_parts_group_note USING (user_id, parts_group_id)
  WHERE first_acquisition_datetime IS NULL
  GROUP BY user_id, parts_group_id;

-- Create a trigger for the ongoing maintenance of i_user_parts_group_note
CREATE TRIGGER i_user_parts_add_group_note
AFTER INSERT ON i_user_parts FOR EACH ROW
BEGIN
  INSERT INTO
    i_user_parts_group_note
    (user_id, parts_group_id, first_acquisition_datetime)
    SELECT
      NEW.user_id,
      parts_group_id,
      NEW.acquisition_datetime
    FROM m_parts
    WHERE m_parts.parts_id = NEW.parts_id
  ON CONFLICT (user_id, parts_group_id) DO NOTHING;
END;
