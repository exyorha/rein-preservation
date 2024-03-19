CREATE VIRTUAL TABLE m_battle_npc_deck_limit_content_deleted_character USING masterdatabase (
  battle_npc_id bigint,
  battle_npc_deck_number integer,
  battle_npc_deck_character_number integer,
  costume_id integer
);

CREATE VIRTUAL TABLE m_costume_delete USING masterdatabase (
  costume_id integer,
  delete_condition_clear_quest_id integer,
  costume_alternative_group_id integer,
  delete_costume_tutorial_type integer,
  material_return_gift_grant_route_type integer
);

CREATE VIRTUAL TABLE m_costume_display_switch USING masterdatabase (
  costume_id integer,
  display_condition_clear_quest_id integer,
  display_deleted_expression_asset_id integer
);

CREATE VIRTUAL TABLE m_library_story_group USING masterdatabase (
  quest_id integer,
  sort_order integer,
  start_quest_scene_id integer,
  end_quest_scene_id integer
);

DROP TABLE m_main_quest_chapter;

CREATE VIRTUAL TABLE m_main_quest_chapter USING masterdatabase (
  main_quest_chapter_id integer,
  main_quest_route_id integer,
  sort_order integer,
  main_quest_sequence_group_id integer,
  portal_cage_character_group_id integer,
  start_datetime timestamp,
  is_invisible_in_library boolean,
  join_library_chapter_id integer
);

CREATE VIRTUAL TABLE m_quest_deck_multi_restriction_group USING masterdatabase (
  quest_deck_multi_restriction_group_id integer,
  group_index integer,
  restriction_value integer
);

CREATE VIRTUAL TABLE m_quest_first_clear_reward_switch USING masterdatabase (
  quest_id integer,
  quest_first_clear_reward_group_id integer,
  switch_condition_clear_quest_id integer
);

CREATE TABLE i_user_deck_limit_content_deleted_character (
  user_id bigint,
  user_deck_number integer,
  user_deck_character_number integer,
  costume_id integer,
  latest_version bigint
);

