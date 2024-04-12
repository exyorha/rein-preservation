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
