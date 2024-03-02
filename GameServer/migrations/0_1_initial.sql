CREATE VIRTUAL TABLE m_ability USING masterdatabase (
  ability_id integer,
  ability_level_group_id integer
);

CREATE VIRTUAL TABLE m_ability_behaviour USING masterdatabase (
  ability_behaviour_id integer,
  ability_behaviour_type integer,
  ability_behaviour_action_id integer
);

CREATE VIRTUAL TABLE m_ability_behaviour_action_bless USING masterdatabase (
  ability_behaviour_action_id integer,
  ability_id integer,
  decrease_point integer
);

CREATE VIRTUAL TABLE m_ability_behaviour_action_passive_skill USING masterdatabase (
  ability_behaviour_action_id integer,
  skill_detail_id integer
);

CREATE VIRTUAL TABLE m_ability_behaviour_action_status USING masterdatabase (
  ability_behaviour_action_id integer,
  ability_behaviour_status_change_type integer,
  attribute_condition_type integer,
  ability_organization_condition_type integer,
  ability_status_id integer,
  apply_scope_type integer
);

CREATE VIRTUAL TABLE m_ability_behaviour_action_status_down USING masterdatabase (
  ability_behaviour_action_id integer,
  ability_behaviour_status_change_type integer,
  attribute_condition_type integer,
  ability_organization_condition_type integer,
  ability_status_id integer,
  apply_scope_type integer
);

CREATE VIRTUAL TABLE m_ability_behaviour_group USING masterdatabase (
  ability_behaviour_group_id integer,
  ability_behaviour_index integer,
  ability_behaviour_id integer
);

CREATE VIRTUAL TABLE m_ability_detail USING masterdatabase (
  ability_detail_id integer,
  name_ability_text_id integer,
  description_ability_text_id integer,
  ability_behaviour_group_id integer,
  asset_category_id integer,
  asset_variation_id integer
);

CREATE VIRTUAL TABLE m_ability_level_group USING masterdatabase (
  ability_level_group_id integer,
  level_lower_limit integer,
  ability_detail_id integer
);

CREATE VIRTUAL TABLE m_ability_status USING masterdatabase (
  ability_status_id integer,
  agility integer,
  attack integer,
  critical_attack_ratio_permil integer,
  critical_ratio_permil integer,
  evasion_ratio_permil integer,
  hp integer,
  vitality integer
);

CREATE VIRTUAL TABLE m_actor USING masterdatabase (
  actor_id integer,
  name_actor_text_id integer,
  actor_asset_id text,
  actor_speaker_icon_asset_path text,
  animator_asset_path text
);

CREATE VIRTUAL TABLE m_actor_animation USING masterdatabase (
  actor_animation_id integer,
  actor_id integer,
  actor_animation_category_id integer,
  actor_animation_type integer,
  asset_path text,
  is_default boolean
);

CREATE VIRTUAL TABLE m_actor_animation_category USING masterdatabase (
  actor_animation_category_id integer,
  name text
);

CREATE VIRTUAL TABLE m_actor_animation_controller USING masterdatabase (
  actor_animation_controller_id integer,
  actor_id integer,
  actor_animation_controller_type integer,
  asset_path text
);

CREATE VIRTUAL TABLE m_actor_object USING masterdatabase (
  actor_object_id integer,
  actor_id integer
);

CREATE VIRTUAL TABLE m_appeal_dialog USING masterdatabase (
  appeal_dialog_id integer,
  sort_order integer,
  appeal_target_type integer,
  appeal_target_id integer,
  start_datetime timestamp,
  end_datetime timestamp,
  title_text_id integer,
  asset_id integer
);

CREATE VIRTUAL TABLE m_asset_background USING masterdatabase (
  asset_background_id integer,
  background_asset_path text,
  global_event_map_asset_path text
);

CREATE VIRTUAL TABLE m_asset_calculator USING masterdatabase (
  asset_calculator_id integer,
  use_calculator_type integer,
  asset_path text
);

CREATE VIRTUAL TABLE m_asset_data_setting USING masterdatabase (
  asset_data_setting_id integer,
  asset_path text
);

CREATE VIRTUAL TABLE m_asset_effect USING masterdatabase (
  asset_effect_id integer,
  asset_path text
);

CREATE VIRTUAL TABLE m_asset_grade_icon USING masterdatabase (
  asset_grade_icon_id integer,
  asset_id integer,
  level integer,
  se_asset_id integer
);

CREATE VIRTUAL TABLE m_asset_timeline USING masterdatabase (
  asset_timeline_id integer,
  asset_path text
);

CREATE VIRTUAL TABLE m_asset_turnbattle_prefab USING masterdatabase (
  asset_turnbattle_prefab_id integer,
  asset_path text
);

CREATE VIRTUAL TABLE m_battle USING masterdatabase (
  battle_id integer,
  battle_npc_id bigint,
  deck_type integer,
  battle_npc_deck_number integer
);

CREATE VIRTUAL TABLE m_battle_actor_ai USING masterdatabase (
  battle_actor_ai_id integer,
  asset_path text,
  description text
);

CREATE VIRTUAL TABLE m_battle_actor_skill_ai_group USING masterdatabase (
  battle_actor_skill_ai_group_id integer,
  priority integer,
  battle_scheme_type integer,
  is_player_side boolean,
  skill_ai_unlock_condition_value_type integer,
  skill_ai_unlock_condition_value integer,
  battle_actor_skill_ai_id integer
);

CREATE VIRTUAL TABLE m_battle_additional_ability USING masterdatabase (
  battle_group_id integer,
  target_actor_appearance_wave_number integer,
  ability_index integer,
  additional_ability_apply_scope_type integer,
  ability_id integer,
  ability_level integer
);

CREATE VIRTUAL TABLE m_battle_attribute_damage_coefficient_define USING masterdatabase (
  battle_scheme_type integer,
  player_attribute_damage_coefficient_group_id integer,
  npc_attribute_damage_coefficient_group_id integer
);

CREATE VIRTUAL TABLE m_battle_attribute_damage_coefficient_group USING masterdatabase (
  attribute_damage_coefficient_group_id integer,
  skill_executor_attribute_type integer,
  skill_target_attribute_type integer,
  attribute_compatibility_type integer,
  damage_coefficient_permil integer
);

CREATE VIRTUAL TABLE m_battle_bgm_set USING masterdatabase (
  bgm_set_id integer,
  track_number integer,
  bgm_asset_id integer,
  stem integer,
  start_wave_number integer
);

CREATE VIRTUAL TABLE m_battle_bgm_set_group USING masterdatabase (
  bgm_set_group_id integer,
  bgm_set_group_index integer,
  bgm_set_id integer,
  random_weight integer
);

CREATE VIRTUAL TABLE m_battle_big_hunt USING masterdatabase (
  battle_group_id integer,
  battle_big_hunt_phase_group_id integer,
  knock_down_gauge_value_config_group_id integer
);

CREATE VIRTUAL TABLE m_battle_big_hunt_damage_threshold_group USING masterdatabase (
  knock_down_damage_threshold_group_id integer,
  knock_down_damage_threshold_group_order integer,
  knock_down_cumulative_damage_threshold integer,
  is_knock_down boolean,
  knock_down_duration_frame_count integer,
  damage_ratio integer
);

CREATE VIRTUAL TABLE m_battle_big_hunt_knock_down_gauge_value_config_group USING masterdatabase (
  knock_down_gauge_value_config_group_id integer,
  active_skill_hit_count integer,
  damage_value_lower_limit integer,
  gauge_value_lower_limit integer,
  correction_ratio_permil integer
);

CREATE VIRTUAL TABLE m_battle_big_hunt_phase_group USING masterdatabase (
  battle_big_hunt_phase_group_id integer,
  battle_big_hunt_phase_group_order integer,
  knock_down_damage_threshold_group_id integer,
  normal_phase_frame_count integer
);

CREATE VIRTUAL TABLE m_battle_companion_skill_ai_group USING masterdatabase (
  battle_companion_skill_ai_group_id integer,
  priority integer,
  battle_scheme_type integer,
  is_player_side boolean,
  skill_ai_unlock_condition_value_type integer,
  skill_ai_unlock_condition_value integer,
  battle_companion_skill_ai_id integer
);

CREATE VIRTUAL TABLE m_battle_costume_skill_fire_act USING masterdatabase (
  costume_id integer,
  battle_skill_fire_act_id integer
);

CREATE VIRTUAL TABLE m_battle_costume_skill_se USING masterdatabase (
  costume_id integer,
  costume_skill_ready_se_asset_id integer
);

CREATE VIRTUAL TABLE m_battle_drop_reward USING masterdatabase (
  battle_drop_reward_id integer,
  possession_type integer,
  possession_id integer,
  count integer
);

CREATE VIRTUAL TABLE m_battle_enemy_size_type_config USING masterdatabase (
  costume_asset_category_type integer,
  actor_skeleton_id integer,
  enemy_size_type integer
);

CREATE VIRTUAL TABLE m_battle_event USING masterdatabase (
  battle_event_id integer,
  battle_event_trigger_behaviour_group_id integer,
  battle_event_receiver_behaviour_group_id integer
);

CREATE VIRTUAL TABLE m_battle_event_group USING masterdatabase (
  battle_event_group_id integer,
  battle_event_id integer
);

CREATE VIRTUAL TABLE m_battle_event_receiver_behaviour_group USING masterdatabase (
  battle_event_receiver_behaviour_group_id integer,
  execute_order integer,
  battle_event_receiver_behaviour_type integer,
  battle_event_receiver_behaviour_id integer
);

CREATE VIRTUAL TABLE m_battle_event_receiver_behaviour_hud_act_sequence USING masterdatabase (
  battle_event_receiver_behaviour_id integer,
  hud_act_sequence_id integer
);

CREATE VIRTUAL TABLE m_battle_event_receiver_behaviour_radio_message USING masterdatabase (
  battle_event_receiver_behaviour_id integer,
  speaker_id integer,
  scenario_key text
);

CREATE VIRTUAL TABLE m_battle_event_trigger_behaviour_battle_start USING masterdatabase (
  battle_event_trigger_behaviour_id integer,
  trigger_on_battle_restore boolean
);

CREATE VIRTUAL TABLE m_battle_event_trigger_behaviour_group USING masterdatabase (
  battle_event_trigger_behaviour_group_id integer,
  battle_event_trigger_behaviour_type integer,
  battle_event_trigger_behaviour_id integer
);

CREATE VIRTUAL TABLE m_battle_event_trigger_behaviour_wave_start USING masterdatabase (
  battle_event_trigger_behaviour_id integer,
  wave_number integer
);

CREATE VIRTUAL TABLE m_battle_general_view_configuration USING masterdatabase (
  quest_scene_id integer,
  wave_number integer,
  is_disable_battle_start_voice boolean,
  is_enable_white_fadeout boolean,
  env_se_id integer,
  wave_win_se_id integer,
  is_disable_play_win_timeline boolean
);

CREATE VIRTUAL TABLE m_battle_group USING masterdatabase (
  battle_group_id integer,
  wave_number integer,
  battle_id integer,
  wave_start_act_asset_id integer,
  wave_end_act_asset_id integer,
  battle_camera_controller_asset_id integer,
  battle_point_index integer,
  battle_start_camera_type integer
);

CREATE VIRTUAL TABLE m_battle_npc USING masterdatabase (
  battle_npc_id bigint
);

CREATE VIRTUAL TABLE m_battle_npc_character USING masterdatabase (
  battle_npc_id bigint,
  character_id integer,
  level integer,
  exp integer
);

CREATE VIRTUAL TABLE m_battle_npc_character_board USING masterdatabase (
  battle_npc_id bigint,
  character_board_id integer,
  panel_release_bit1 integer,
  panel_release_bit2 integer,
  panel_release_bit3 integer,
  panel_release_bit4 integer
);

CREATE VIRTUAL TABLE m_battle_npc_character_board_ability USING masterdatabase (
  battle_npc_id bigint,
  character_id integer,
  ability_id integer,
  level integer
);

CREATE VIRTUAL TABLE m_battle_npc_character_board_complete_reward USING masterdatabase (
  battle_npc_id bigint,
  character_board_complete_reward_id integer
);

CREATE VIRTUAL TABLE m_battle_npc_character_board_status_up USING masterdatabase (
  battle_npc_id bigint,
  character_id integer,
  status_calculation_type integer,
  hp integer,
  attack integer,
  vitality integer,
  agility integer,
  critical_ratio integer,
  critical_attack integer
);

CREATE VIRTUAL TABLE m_battle_npc_character_costume_level_bonus USING masterdatabase (
  battle_npc_id bigint,
  character_id integer,
  status_calculation_type integer,
  hp integer,
  attack integer,
  vitality integer,
  agility integer,
  critical_ratio integer,
  critical_attack integer
);

CREATE VIRTUAL TABLE m_battle_npc_character_rebirth USING masterdatabase (
  battle_npc_id bigint,
  character_id integer,
  rebirth_count integer
);

CREATE VIRTUAL TABLE m_battle_npc_character_viewer_field USING masterdatabase (
  battle_npc_id bigint,
  character_viewer_field_id integer,
  release_datetime timestamp
);

CREATE VIRTUAL TABLE m_battle_npc_companion USING masterdatabase (
  battle_npc_id bigint,
  battle_npc_companion_uuid text,
  companion_id integer,
  headup_display_view_id integer,
  level integer,
  acquisition_datetime timestamp
);

CREATE VIRTUAL TABLE m_battle_npc_costume USING masterdatabase (
  battle_npc_id bigint,
  battle_npc_costume_uuid text,
  costume_id integer,
  limit_break_count integer,
  level integer,
  exp integer,
  headup_display_view_id integer,
  acquisition_datetime timestamp,
  awaken_count integer
);

CREATE VIRTUAL TABLE m_battle_npc_costume_active_skill USING masterdatabase (
  battle_npc_id bigint,
  battle_npc_costume_uuid text,
  level integer,
  acquisition_datetime timestamp
);

CREATE VIRTUAL TABLE m_battle_npc_costume_awaken_status_up USING masterdatabase (
  battle_npc_id bigint,
  battle_npc_costume_uuid text,
  status_calculation_type integer,
  hp integer,
  attack integer,
  vitality integer,
  agility integer,
  critical_ratio integer,
  critical_attack integer
);

CREATE VIRTUAL TABLE m_battle_npc_costume_level_bonus_reevaluate USING masterdatabase (
  battle_npc_id bigint,
  last_reevaluate_datetime timestamp
);

CREATE VIRTUAL TABLE m_battle_npc_costume_level_bonus_release_status USING masterdatabase (
  battle_npc_id bigint,
  costume_id integer,
  last_released_bonus_level integer,
  confirmed_bonus_level integer
);

CREATE VIRTUAL TABLE m_battle_npc_costume_lottery_effect USING masterdatabase (
  battle_npc_id bigint,
  battle_npc_costume_uuid text,
  slot_number integer,
  odds_number integer
);

CREATE VIRTUAL TABLE m_battle_npc_costume_lottery_effect_ability USING masterdatabase (
  battle_npc_id bigint,
  battle_npc_costume_uuid text,
  slot_number integer,
  ability_id integer,
  ability_level integer
);

CREATE VIRTUAL TABLE m_battle_npc_costume_lottery_effect_pending USING masterdatabase (
  battle_npc_id bigint,
  battle_npc_costume_uuid text,
  slot_number integer,
  odds_number integer
);

CREATE VIRTUAL TABLE m_battle_npc_costume_lottery_effect_status_up USING masterdatabase (
  battle_npc_id bigint,
  battle_npc_costume_uuid text,
  status_calculation_type integer,
  hp integer,
  attack integer,
  vitality integer,
  agility integer,
  critical_ratio integer,
  critical_attack integer
);

CREATE VIRTUAL TABLE m_battle_npc_deck USING masterdatabase (
  battle_npc_id bigint,
  deck_type integer,
  battle_npc_deck_number integer,
  battle_npc_deck_character_uuid01 text,
  battle_npc_deck_character_uuid02 text,
  battle_npc_deck_character_uuid03 text,
  name text,
  power integer
);

CREATE VIRTUAL TABLE m_battle_npc_deck_backup USING masterdatabase (
  battle_npc_id bigint,
  battle_npc_deck_backup_uuid text,
  deck_json text
);

CREATE VIRTUAL TABLE m_battle_npc_deck_character USING masterdatabase (
  battle_npc_id bigint,
  battle_npc_deck_character_uuid text,
  battle_npc_costume_uuid text,
  main_battle_npc_weapon_uuid text,
  battle_npc_companion_uuid text,
  power integer,
  battle_npc_thought_uuid text
);

CREATE VIRTUAL TABLE m_battle_npc_deck_character_dressup_costume USING masterdatabase (
  battle_npc_id bigint,
  battle_npc_deck_character_uuid text,
  dressup_costume_id integer
);

CREATE VIRTUAL TABLE m_battle_npc_deck_character_drop_category USING masterdatabase (
  battle_npc_id bigint,
  battle_npc_deck_character_uuid text,
  battle_drop_category_id integer
);

CREATE VIRTUAL TABLE m_battle_npc_deck_character_type USING masterdatabase (
  battle_npc_id bigint,
  battle_npc_deck_character_uuid text,
  battle_enemy_type integer
);

CREATE VIRTUAL TABLE m_battle_npc_deck_limit_content_backup USING masterdatabase (
  battle_npc_id bigint,
  event_quest_chapter_id integer,
  event_quest_sequence_sort_order integer,
  battle_npc_deck_backup_uuid text
);

CREATE VIRTUAL TABLE m_battle_npc_deck_limit_content_backup_restored USING masterdatabase (
  battle_npc_id bigint,
  event_quest_chapter_id integer,
  difficulty_type integer
);

CREATE VIRTUAL TABLE m_battle_npc_deck_limit_content_restricted USING masterdatabase (
  battle_npc_id bigint,
  event_quest_chapter_id integer,
  quest_id integer,
  deck_restricted_uuid text,
  possession_type integer,
  target_uuid text
);

CREATE VIRTUAL TABLE m_battle_npc_deck_parts_group USING masterdatabase (
  battle_npc_id bigint,
  battle_npc_deck_character_uuid text,
  battle_npc_parts_uuid text,
  sort_order integer
);

CREATE VIRTUAL TABLE m_battle_npc_deck_sub_weapon_group USING masterdatabase (
  battle_npc_id bigint,
  battle_npc_deck_character_uuid text,
  battle_npc_weapon_uuid text,
  sort_order integer
);

CREATE VIRTUAL TABLE m_battle_npc_deck_type_note USING masterdatabase (
  battle_npc_id bigint,
  deck_type integer,
  max_deck_power integer
);

CREATE VIRTUAL TABLE m_battle_npc_parts USING masterdatabase (
  battle_npc_id bigint,
  battle_npc_parts_uuid text,
  parts_id integer,
  level integer,
  parts_status_main_id integer,
  is_protected boolean,
  acquisition_datetime timestamp
);

CREATE VIRTUAL TABLE m_battle_npc_parts_group_note USING masterdatabase (
  battle_npc_id bigint,
  parts_group_id integer,
  first_acquisition_datetime timestamp
);

CREATE VIRTUAL TABLE m_battle_npc_parts_preset USING masterdatabase (
  battle_npc_id bigint,
  battle_npc_parts_preset_number integer,
  battle_npc_parts_uuid01 text,
  battle_npc_parts_uuid02 text,
  battle_npc_parts_uuid03 text,
  name text,
  battle_npc_parts_preset_tag_number integer
);

CREATE VIRTUAL TABLE m_battle_npc_parts_preset_tag USING masterdatabase (
  battle_npc_id bigint,
  battle_npc_parts_preset_tag_number integer,
  name text
);

CREATE VIRTUAL TABLE m_battle_npc_parts_status_sub USING masterdatabase (
  battle_npc_id bigint,
  battle_npc_parts_uuid text,
  status_index integer,
  parts_status_sub_lottery_id integer,
  level integer,
  status_kind_type integer,
  status_calculation_type integer,
  status_change_value integer
);

CREATE VIRTUAL TABLE m_battle_npc_special_end_act USING masterdatabase (
  quest_scene_id integer,
  wave_number integer,
  battle_npc_id bigint,
  battle_npc_deck_character_uuid text,
  special_end_battle_act_type integer
);

CREATE VIRTUAL TABLE m_battle_npc_weapon USING masterdatabase (
  battle_npc_id bigint,
  battle_npc_weapon_uuid text,
  weapon_id integer,
  level integer,
  exp integer,
  limit_break_count integer,
  is_protected boolean,
  acquisition_datetime timestamp
);

CREATE VIRTUAL TABLE m_battle_npc_weapon_ability USING masterdatabase (
  battle_npc_id bigint,
  battle_npc_weapon_uuid text,
  slot_number integer,
  level integer
);

CREATE VIRTUAL TABLE m_battle_npc_weapon_ability_reevaluate USING masterdatabase (
  battle_npc_id bigint,
  last_reevaluate_datetime timestamp
);

CREATE VIRTUAL TABLE m_battle_npc_weapon_awaken USING masterdatabase (
  battle_npc_id bigint,
  battle_npc_weapon_uuid text
);

CREATE VIRTUAL TABLE m_battle_npc_weapon_note USING masterdatabase (
  battle_npc_id bigint,
  weapon_id integer,
  max_level integer,
  max_limit_break_count integer,
  first_acquisition_datetime timestamp
);

CREATE VIRTUAL TABLE m_battle_npc_weapon_note_reevaluate USING masterdatabase (
  battle_npc_id bigint,
  last_reevaluate_datetime timestamp
);

CREATE VIRTUAL TABLE m_battle_npc_weapon_skill USING masterdatabase (
  battle_npc_id bigint,
  battle_npc_weapon_uuid text,
  slot_number integer,
  level integer
);

CREATE VIRTUAL TABLE m_battle_npc_weapon_story USING masterdatabase (
  battle_npc_id bigint,
  weapon_id integer,
  released_max_story_index integer
);

CREATE VIRTUAL TABLE m_battle_npc_weapon_story_reevaluate USING masterdatabase (
  battle_npc_id bigint,
  last_reevaluate_datetime timestamp
);

CREATE VIRTUAL TABLE m_battle_progress_ui_type USING masterdatabase (
  quest_scene_id integer,
  battle_progress_ui_type_id integer
);

CREATE VIRTUAL TABLE m_battle_quest_scene_bgm USING masterdatabase (
  quest_scene_id integer,
  start_wave_number integer,
  bgm_id integer,
  stem integer,
  track_number integer
);

CREATE VIRTUAL TABLE m_battle_quest_scene_bgm_set_group USING masterdatabase (
  quest_scene_id integer,
  bgm_set_group_id integer
);

CREATE VIRTUAL TABLE m_battle_rental_deck USING masterdatabase (
  battle_group_id integer,
  battle_npc_id bigint,
  deck_type integer,
  battle_npc_deck_number integer
);

CREATE VIRTUAL TABLE m_battle_skill_behaviour_hit_damage_configuration USING masterdatabase (
  skill_category_type integer,
  hit_count integer,
  hit_index_lower_limit integer,
  damage_coefficient_value_permil integer
);

CREATE VIRTUAL TABLE m_battle_skill_fire_act USING masterdatabase (
  battle_skill_fire_act_id integer,
  battle_skill_fire_act_condition_group_id integer,
  battle_skill_fire_act_condition_group_type integer,
  battle_skill_fire_act_asset_id integer
);

CREATE VIRTUAL TABLE m_battle_skill_fire_act_condition_attribute_type USING masterdatabase (
  battle_skill_fire_act_condition_id integer,
  attribute_type integer
);

CREATE VIRTUAL TABLE m_battle_skill_fire_act_condition_group USING masterdatabase (
  battle_skill_fire_act_condition_group_id integer,
  battle_skill_fire_act_condition_group_order integer,
  battle_skill_fire_act_condition_type integer,
  battle_skill_fire_act_condition_id integer
);

CREATE VIRTUAL TABLE m_battle_skill_fire_act_condition_skill_category_type USING masterdatabase (
  battle_skill_fire_act_condition_id integer,
  skill_category_type integer
);

CREATE VIRTUAL TABLE m_battle_skill_fire_act_condition_weapon_type USING masterdatabase (
  battle_skill_fire_act_condition_id integer,
  weapon_type integer
);

CREATE VIRTUAL TABLE m_beginner_campaign USING masterdatabase (
  beginner_campaign_id integer,
  beginner_judge_start_datetime timestamp,
  beginner_judge_end_datetime timestamp,
  grant_campaign_term_day_count integer,
  campaign_unlock_quest_id integer
);

CREATE VIRTUAL TABLE m_big_hunt_boss USING masterdatabase (
  big_hunt_boss_id integer,
  big_hunt_boss_grade_group_id integer,
  name_big_hunt_boss_text_id integer,
  big_hunt_boss_asset_id integer,
  attribute_type integer
);

CREATE VIRTUAL TABLE m_big_hunt_boss_grade_group USING masterdatabase (
  big_hunt_boss_grade_group_id integer,
  necessary_score bigint,
  asset_grade_icon_id integer
);

CREATE VIRTUAL TABLE m_big_hunt_boss_grade_group_attribute USING masterdatabase (
  attribute_type integer,
  big_hunt_boss_grade_group_id integer
);

CREATE VIRTUAL TABLE m_big_hunt_boss_quest USING masterdatabase (
  big_hunt_boss_quest_id integer,
  big_hunt_boss_id integer,
  big_hunt_quest_group_id integer,
  big_hunt_boss_quest_score_coefficient_id integer,
  big_hunt_score_reward_group_schedule_id integer,
  big_hunt_link_id integer,
  daily_challenge_count integer
);

CREATE VIRTUAL TABLE m_big_hunt_boss_quest_group USING masterdatabase (
  big_hunt_boss_quest_group_id integer,
  sort_order integer,
  big_hunt_boss_quest_id integer
);

CREATE VIRTUAL TABLE m_big_hunt_boss_quest_group_challenge_category USING masterdatabase (
  big_hunt_boss_quest_group_challenge_category_id integer,
  sort_order integer,
  big_hunt_boss_quest_group_id integer
);

CREATE VIRTUAL TABLE m_big_hunt_link USING masterdatabase (
  big_hunt_link_id integer,
  destination_domain_type integer,
  destination_domain_id integer,
  possession_type integer,
  possession_id integer
);

CREATE VIRTUAL TABLE m_big_hunt_quest USING masterdatabase (
  big_hunt_quest_id integer,
  quest_id integer,
  big_hunt_quest_score_coefficient_id integer
);

CREATE VIRTUAL TABLE m_big_hunt_quest_group USING masterdatabase (
  big_hunt_quest_group_id integer,
  sort_order integer,
  big_hunt_quest_id integer
);

CREATE VIRTUAL TABLE m_big_hunt_quest_score_coefficient USING masterdatabase (
  big_hunt_quest_score_coefficient_id integer,
  score_difficult_bonus_permil integer
);

CREATE VIRTUAL TABLE m_big_hunt_reward_group USING masterdatabase (
  big_hunt_reward_group_id integer,
  sort_order integer,
  possession_type integer,
  possession_id integer,
  count integer
);

CREATE VIRTUAL TABLE m_big_hunt_schedule USING masterdatabase (
  big_hunt_schedule_id integer,
  notice_start_datetime timestamp,
  challenge_start_datetime timestamp,
  challenge_end_datetime timestamp,
  season_asset_id integer
);

CREATE VIRTUAL TABLE m_big_hunt_score_reward_group USING masterdatabase (
  big_hunt_score_reward_group_id integer,
  necessary_score bigint,
  big_hunt_reward_group_id integer
);

CREATE VIRTUAL TABLE m_big_hunt_score_reward_group_schedule USING masterdatabase (
  big_hunt_score_reward_group_schedule_id integer,
  group_index integer,
  big_hunt_score_reward_group_id integer,
  start_datetime timestamp
);

CREATE VIRTUAL TABLE m_big_hunt_weekly_attribute_score_reward_group_schedule USING masterdatabase (
  big_hunt_weekly_attribute_score_reward_group_schedule_id integer,
  attribute_type integer,
  group_index integer,
  big_hunt_score_reward_group_id integer,
  start_datetime timestamp
);

CREATE VIRTUAL TABLE m_cage_memory USING masterdatabase (
  cage_memory_id integer,
  main_quest_season_id integer,
  sort_order integer,
  cage_memory_asset_id integer
);

CREATE VIRTUAL TABLE m_cage_ornament USING masterdatabase (
  cage_ornament_id integer,
  start_datetime timestamp,
  end_datetime timestamp,
  cage_ornament_reward_id integer
);

CREATE VIRTUAL TABLE m_cage_ornament_main_quest_chapter_still USING masterdatabase (
  main_quest_chapter_id integer,
  cage_ornament_still_release_condition_id integer
);

CREATE VIRTUAL TABLE m_cage_ornament_reward USING masterdatabase (
  cage_ornament_reward_id integer,
  possession_type integer,
  possession_id integer,
  count integer
);

CREATE VIRTUAL TABLE m_cage_ornament_still_release_condition USING masterdatabase (
  cage_ornament_still_release_condition_id integer,
  cage_ornament_id integer
);

CREATE VIRTUAL TABLE m_catalog_companion USING masterdatabase (
  companion_id integer,
  sort_order integer,
  catalog_term_id integer
);

CREATE VIRTUAL TABLE m_catalog_costume USING masterdatabase (
  costume_id integer,
  sort_order integer,
  catalog_term_id integer
);

CREATE VIRTUAL TABLE m_catalog_parts_group USING masterdatabase (
  parts_group_id integer,
  sort_order integer,
  catalog_term_id integer
);

CREATE VIRTUAL TABLE m_catalog_term USING masterdatabase (
  catalog_term_id integer,
  start_datetime timestamp
);

CREATE VIRTUAL TABLE m_catalog_thought USING masterdatabase (
  thought_id integer,
  sort_order integer,
  catalog_term_id integer
);

CREATE VIRTUAL TABLE m_catalog_weapon USING masterdatabase (
  weapon_id integer,
  sort_order integer,
  catalog_term_id integer
);

CREATE VIRTUAL TABLE m_character USING masterdatabase (
  character_id integer,
  character_level_bonus_ability_group_id integer,
  name_character_text_id integer,
  character_asset_id integer,
  sort_order integer,
  default_costume_id integer,
  default_weapon_id integer,
  end_costume_id integer,
  end_weapon_id integer,
  max_level_numerical_function_id integer,
  required_exp_for_level_up_numerical_parameter_map_id integer,
  list_setting_costume_group_type integer,
  list_setting_display_start_datetime timestamp
);

CREATE VIRTUAL TABLE m_character_board USING masterdatabase (
  character_board_id integer,
  character_board_group_id integer,
  character_board_unlock_condition_group_id integer,
  release_rank integer
);

CREATE VIRTUAL TABLE m_character_board_ability USING masterdatabase (
  character_board_ability_id integer,
  character_board_effect_target_group_id integer,
  ability_id integer
);

CREATE VIRTUAL TABLE m_character_board_ability_max_level USING masterdatabase (
  character_id integer,
  ability_id integer,
  max_level integer
);

CREATE VIRTUAL TABLE m_character_board_assignment USING masterdatabase (
  character_id integer,
  character_board_category_id integer,
  sort_order integer,
  character_board_assignment_type integer
);

CREATE VIRTUAL TABLE m_character_board_category USING masterdatabase (
  character_board_category_id integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_character_board_complete_reward USING masterdatabase (
  character_board_complete_reward_id integer,
  character_board_complete_reward_group_id integer,
  character_board_complete_reward_condition_group_id integer
);

CREATE VIRTUAL TABLE m_character_board_complete_reward_group USING masterdatabase (
  character_board_complete_reward_group_id integer,
  possession_type integer,
  possession_id integer,
  count integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_character_board_condition USING masterdatabase (
  character_board_condition_group_id integer,
  group_index integer,
  character_board_condition_type integer,
  character_board_condition_detail_id integer,
  character_board_condition_ignore_id integer,
  condition_value integer
);

CREATE VIRTUAL TABLE m_character_board_condition_detail USING masterdatabase (
  character_board_condition_detail_id integer,
  detail_index integer,
  condition_value integer
);

CREATE VIRTUAL TABLE m_character_board_condition_group USING masterdatabase (
  character_board_condition_group_id integer,
  condition_operation_type integer
);

CREATE VIRTUAL TABLE m_character_board_condition_ignore USING masterdatabase (
  character_board_condition_ignore_id integer,
  ignore_index integer,
  condition_value integer
);

CREATE VIRTUAL TABLE m_character_board_effect_target_group USING masterdatabase (
  character_board_effect_target_group_id integer,
  group_index integer,
  character_board_effect_target_type integer,
  target_value integer
);

CREATE VIRTUAL TABLE m_character_board_group USING masterdatabase (
  character_board_group_id integer,
  character_board_category_id integer,
  sort_order integer,
  character_board_group_type integer,
  text_asset_id integer
);

CREATE VIRTUAL TABLE m_character_board_panel USING masterdatabase (
  character_board_panel_id integer,
  character_board_id integer,
  character_board_panel_unlock_condition_group_id integer,
  character_board_panel_release_possession_group_id integer,
  character_board_panel_release_reward_group_id integer,
  character_board_panel_release_effect_group_id integer,
  sort_order integer,
  parent_character_board_panel_id integer,
  place_index integer
);

CREATE VIRTUAL TABLE m_character_board_panel_release_effect_group USING masterdatabase (
  character_board_panel_release_effect_group_id integer,
  sort_order integer,
  character_board_effect_type integer,
  character_board_effect_id integer,
  effect_value integer
);

CREATE VIRTUAL TABLE m_character_board_panel_release_possession_group USING masterdatabase (
  character_board_panel_release_possession_group_id integer,
  possession_type integer,
  possession_id integer,
  count integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_character_board_panel_release_reward_group USING masterdatabase (
  character_board_panel_release_reward_group_id integer,
  possession_type integer,
  possession_id integer,
  count integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_character_board_status_up USING masterdatabase (
  character_board_status_up_id integer,
  character_board_status_up_type integer,
  character_board_effect_target_group_id integer
);

CREATE VIRTUAL TABLE m_character_display_switch USING masterdatabase (
  character_id integer,
  name_character_text_id integer,
  default_costume_id integer,
  default_weapon_id integer,
  display_condition_clear_quest_id integer,
  character_asset_id integer
);

CREATE VIRTUAL TABLE m_character_level_bonus_ability_group USING masterdatabase (
  character_level_bonus_ability_group_id integer,
  activation_character_level integer,
  ability_id integer,
  ability_level integer
);

CREATE VIRTUAL TABLE m_character_rebirth USING masterdatabase (
  character_id integer,
  character_rebirth_step_group_id integer,
  sort_order integer,
  character_assignment_type integer
);

CREATE VIRTUAL TABLE m_character_rebirth_material_group USING masterdatabase (
  character_rebirth_material_group_id integer,
  material_id integer,
  count integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_character_rebirth_step_group USING masterdatabase (
  character_rebirth_step_group_id integer,
  before_rebirth_count integer,
  costume_level_limit_up integer,
  character_rebirth_material_group_id integer
);

CREATE VIRTUAL TABLE m_character_viewer_actor_icon USING masterdatabase (
  costume_asset_category_type integer,
  skeleton_id integer,
  asset_variation_id integer,
  override_costume_asset_category_type integer,
  override_icon_skeleton_id integer,
  override_icon_asset_variation_id integer
);

CREATE VIRTUAL TABLE m_character_viewer_field USING masterdatabase (
  character_viewer_field_id integer,
  release_evaluate_condition_id integer,
  publish_datetime timestamp,
  character_viewer_field_asset_id integer,
  asset_background_id integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_character_viewer_field_settings USING masterdatabase (
  asset_background_id integer,
  bgm_asset_id integer,
  stem integer,
  battle_field_locale_setting_index integer,
  post_process_configuration_index integer,
  battle_point_index integer
);

CREATE VIRTUAL TABLE m_character_voice_unlock_condition USING masterdatabase (
  character_id integer,
  sort_order integer,
  character_voice_unlock_condition_type integer,
  condition_value integer,
  voice_asset_id integer
);

CREATE VIRTUAL TABLE m_collection_bonus_effect USING masterdatabase (
  collection_bonus_effect_id integer,
  collection_bonus_effect_type integer,
  amount00 integer,
  amount01 integer,
  amount02 integer,
  amount03 integer,
  amount04 integer
);

CREATE VIRTUAL TABLE m_collection_bonus_quest_assignment USING masterdatabase (
  collection_bonus_quest_assignment_id integer,
  quest_assignment_type integer,
  main_quest_chapter_id integer,
  event_quest_chapter_id integer,
  quest_id integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_collection_bonus_quest_assignment_group USING masterdatabase (
  collection_bonus_quest_assignment_group_id integer,
  collection_bonus_quest_assignment_id integer
);

CREATE VIRTUAL TABLE m_combo_calculation_setting USING masterdatabase (
  combo_count_lower_limit integer,
  damage_coefficient_permil integer,
  ui_effect_index integer
);

CREATE VIRTUAL TABLE m_comeback_campaign USING masterdatabase (
  comeback_campaign_id integer,
  comeback_judge_start_datetime timestamp,
  comeback_judge_end_datetime timestamp,
  comeback_judge_day_count integer,
  grant_campaign_term_day_count integer,
  campaign_unlock_quest_id integer,
  comeback_campaign_grade_group_id integer
);

CREATE VIRTUAL TABLE m_companion USING masterdatabase (
  companion_id integer,
  attribute_type integer,
  companion_category_type integer,
  companion_base_status_id integer,
  companion_status_calculation_id integer,
  skill_id integer,
  companion_ability_group_id integer,
  actor_id integer,
  actor_skeleton_id integer,
  asset_variation_id integer,
  character_mover_battle_actor_ai_id integer
);

CREATE VIRTUAL TABLE m_companion_ability_group USING masterdatabase (
  companion_ability_group_id integer,
  slot_number integer,
  ability_id integer
);

CREATE VIRTUAL TABLE m_companion_ability_level USING masterdatabase (
  companion_level_lower_limit integer,
  ability_level integer
);

CREATE VIRTUAL TABLE m_companion_base_status USING masterdatabase (
  companion_base_status_id integer,
  attack integer,
  hp integer,
  vitality integer
);

CREATE VIRTUAL TABLE m_companion_category USING masterdatabase (
  companion_category_type integer,
  enhancement_cost_numerical_function_id integer
);

CREATE VIRTUAL TABLE m_companion_duplication_exchange_possession_group USING masterdatabase (
  companion_id integer,
  possession_type integer,
  possession_id integer,
  count integer
);

CREATE VIRTUAL TABLE m_companion_enhanced USING masterdatabase (
  companion_enhanced_id integer,
  companion_id integer,
  level integer
);

CREATE VIRTUAL TABLE m_companion_enhancement_material USING masterdatabase (
  companion_category_type integer,
  level integer,
  material_id integer,
  count integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_companion_skill_level USING masterdatabase (
  companion_level_lower_limit integer,
  skill_level integer
);

CREATE VIRTUAL TABLE m_companion_status_calculation USING masterdatabase (
  companion_status_calculation_id integer,
  attack_numerical_function_id integer,
  hp_numerical_function_id integer,
  vitality_numerical_function_id integer
);

CREATE VIRTUAL TABLE m_complete_mission_group USING masterdatabase (
  mission_id integer,
  possession_type integer,
  possession_id integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_config USING masterdatabase (
  config_key text,
  value text
);

CREATE VIRTUAL TABLE m_consumable_item USING masterdatabase (
  consumable_item_id integer,
  consumable_item_type integer,
  sort_order integer,
  sell_price integer,
  consumable_item_term_id integer,
  asset_name text,
  asset_category_id integer,
  asset_variation_id integer
);

CREATE VIRTUAL TABLE m_consumable_item_effect USING masterdatabase (
  consumable_item_id integer,
  effect_target_type integer,
  effect_value_type integer,
  effect_value integer
);

CREATE VIRTUAL TABLE m_consumable_item_term USING masterdatabase (
  consumable_item_term_id integer,
  start_datetime timestamp,
  end_datetime timestamp
);

CREATE VIRTUAL TABLE m_contents_story USING masterdatabase (
  contents_story_id integer,
  quest_scene_type integer,
  asset_background_id integer,
  event_map_number_upper integer,
  event_map_number_lower integer,
  is_forced_play boolean,
  contents_story_unlock_condition_type integer,
  condition_value integer,
  unlock_evaluate_condition_id integer
);

CREATE VIRTUAL TABLE m_costume USING masterdatabase (
  costume_id integer,
  character_id integer,
  actor_id integer,
  costume_asset_category_type integer,
  actor_skeleton_id integer,
  asset_variation_id integer,
  skillful_weapon_type integer,
  rarity_type integer,
  costume_base_status_id integer,
  costume_status_calculation_id integer,
  costume_limit_break_material_group_id integer,
  costume_ability_group_id integer,
  costume_active_skill_group_id integer,
  counter_skill_detail_id integer,
  character_mover_battle_actor_ai_id integer,
  costume_default_skill_group_id integer,
  costume_level_bonus_id integer,
  default_actor_skill_ai_id integer,
  costume_emblem_asset_id integer,
  battle_actor_skill_ai_group_id integer
);

CREATE VIRTUAL TABLE m_costume_ability_group USING masterdatabase (
  costume_ability_group_id integer,
  slot_number integer,
  ability_id integer,
  costume_ability_level_group_id integer
);

CREATE VIRTUAL TABLE m_costume_ability_level_group USING masterdatabase (
  costume_ability_level_group_id integer,
  costume_limit_break_count_lower_limit integer,
  ability_level integer
);

CREATE VIRTUAL TABLE m_costume_active_skill_enhancement_material USING masterdatabase (
  costume_active_skill_enhancement_material_id integer,
  skill_level integer,
  material_id integer,
  count integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_costume_active_skill_group USING masterdatabase (
  costume_active_skill_group_id integer,
  costume_limit_break_count_lower_limit integer,
  costume_active_skill_id integer,
  costume_active_skill_enhancement_material_id integer
);

CREATE VIRTUAL TABLE m_costume_animation_step USING masterdatabase (
  costume_id integer,
  step integer,
  actor_animation_id integer
);

CREATE VIRTUAL TABLE m_costume_auto_organization_condition USING masterdatabase (
  costume_id integer,
  costume_auto_organization_condition_type integer,
  target_value integer
);

CREATE VIRTUAL TABLE m_costume_awaken USING masterdatabase (
  costume_id integer,
  costume_awaken_effect_group_id integer,
  costume_awaken_step_material_group_id integer,
  costume_awaken_price_group_id integer
);

CREATE VIRTUAL TABLE m_costume_awaken_ability USING masterdatabase (
  costume_awaken_ability_id integer,
  ability_id integer,
  ability_level integer
);

CREATE VIRTUAL TABLE m_costume_awaken_effect_group USING masterdatabase (
  costume_awaken_effect_group_id integer,
  awaken_step integer,
  costume_awaken_effect_type integer,
  costume_awaken_effect_id integer
);

CREATE VIRTUAL TABLE m_costume_awaken_item_acquire USING masterdatabase (
  costume_awaken_item_acquire_id integer,
  possession_type integer,
  possession_id integer,
  count integer
);

CREATE VIRTUAL TABLE m_costume_awaken_material_group USING masterdatabase (
  costume_awaken_material_group_id integer,
  material_id integer,
  count integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_costume_awaken_price_group USING masterdatabase (
  costume_awaken_price_group_id integer,
  awaken_step_lower_limit integer,
  gold integer
);

CREATE VIRTUAL TABLE m_costume_awaken_status_up_group USING masterdatabase (
  costume_awaken_status_up_group_id integer,
  sort_order integer,
  status_kind_type integer,
  status_calculation_type integer,
  effect_value integer
);

CREATE VIRTUAL TABLE m_costume_awaken_step_material_group USING masterdatabase (
  costume_awaken_step_material_group_id integer,
  awaken_step_lower_limit integer,
  costume_awaken_material_group_id integer
);

CREATE VIRTUAL TABLE m_costume_base_status USING masterdatabase (
  costume_base_status_id integer,
  hp integer,
  attack integer,
  vitality integer,
  agility integer,
  critical_ratio_permil integer,
  critical_attack_ratio_permil integer,
  evasion_ratio_permil integer
);

CREATE VIRTUAL TABLE m_costume_collection_bonus USING masterdatabase (
  collection_bonus_id integer,
  collection_bonus_text_id integer,
  collection_bonus_group_id integer,
  collection_bonus_quest_assignment_group_id integer,
  collection_bonus_effect_id integer,
  start_datetime timestamp,
  end_datetime timestamp,
  grouping_id integer
);

CREATE VIRTUAL TABLE m_costume_collection_bonus_group USING masterdatabase (
  collection_bonus_group_id integer,
  costume_id integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_costume_default_skill_group USING masterdatabase (
  costume_default_skill_group_id integer,
  costume_default_skill_lottery_type integer,
  costume_default_skill_lottery_group_id integer
);

CREATE VIRTUAL TABLE m_costume_default_skill_lottery_group USING masterdatabase (
  costume_default_skill_lottery_group_id integer,
  skill_detail_id integer,
  probability_weight integer
);

CREATE VIRTUAL TABLE m_costume_display_coordinate_adjustment USING masterdatabase (
  costume_id integer,
  display_coordinate_adjustment_function_type integer,
  horizontal_coordinate_count_permil integer,
  vertical_coordinate_count_permil integer
);

CREATE VIRTUAL TABLE m_costume_duplication_exchange_possession_group USING masterdatabase (
  costume_id integer,
  possession_type integer,
  possession_id integer,
  count integer
);

CREATE VIRTUAL TABLE m_costume_emblem USING masterdatabase (
  costume_emblem_asset_id integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_costume_enhanced USING masterdatabase (
  costume_enhanced_id integer,
  costume_id integer,
  limit_break_count integer,
  level integer,
  active_skill_level integer,
  awaken_count integer
);

CREATE VIRTUAL TABLE m_costume_level_bonus USING masterdatabase (
  costume_level_bonus_id integer,
  level integer,
  costume_level_bonus_type integer,
  effect_value integer
);

CREATE VIRTUAL TABLE m_costume_limit_break_material_group USING masterdatabase (
  costume_limit_break_material_group_id integer,
  material_id integer,
  count integer,
  sort_order integer,
  costume_overflow_exchange_possession_group_id integer
);

CREATE VIRTUAL TABLE m_costume_limit_break_material_rarity_group USING masterdatabase (
  costume_limit_break_material_rarity_group_id integer,
  material_id integer,
  count integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_costume_lottery_effect USING masterdatabase (
  costume_id integer,
  slot_number integer,
  costume_lottery_effect_odds_group_id integer,
  costume_lottery_effect_unlock_material_group_id integer,
  costume_lottery_effect_draw_material_group_id integer,
  costume_lottery_effect_release_schedule_id integer
);

CREATE VIRTUAL TABLE m_costume_lottery_effect_material_group USING masterdatabase (
  costume_lottery_effect_material_group_id integer,
  material_id integer,
  count integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_costume_lottery_effect_odds_group USING masterdatabase (
  costume_lottery_effect_odds_group_id integer,
  odds_number integer,
  weight integer,
  costume_lottery_effect_type integer,
  costume_lottery_effect_target_id integer,
  rarity_type integer
);

CREATE VIRTUAL TABLE m_costume_lottery_effect_release_schedule USING masterdatabase (
  costume_lottery_effect_release_schedule_id integer,
  release_datetime timestamp
);

CREATE VIRTUAL TABLE m_costume_lottery_effect_target_ability USING masterdatabase (
  costume_lottery_effect_target_ability_id integer,
  ability_id integer,
  ability_level integer
);

CREATE VIRTUAL TABLE m_costume_lottery_effect_target_status_up USING masterdatabase (
  costume_lottery_effect_target_status_up_id integer,
  status_kind_type integer,
  status_calculation_type integer,
  effect_value integer
);

CREATE VIRTUAL TABLE m_costume_overflow_exchange_possession_group USING masterdatabase (
  material_id integer,
  sort_order integer,
  possession_type integer,
  possession_id integer,
  count integer
);

CREATE VIRTUAL TABLE m_costume_proper_attribute_hp_bonus USING masterdatabase (
  costume_id integer,
  costume_proper_attribute_type integer,
  main_weapon_hp_additional_value integer,
  sub_weapon_hp_additional_value integer
);

CREATE VIRTUAL TABLE m_costume_rarity USING masterdatabase (
  rarity_type integer,
  costume_limit_break_material_rarity_group_id integer,
  enhancement_cost_by_material_numerical_function_id integer,
  limit_break_cost_numerical_function_id integer,
  max_level_numerical_function_id integer,
  required_exp_for_level_up_numerical_parameter_map_id integer,
  active_skill_max_level_numerical_function_id integer,
  active_skill_enhancement_cost_numerical_function_id integer
);

CREATE VIRTUAL TABLE m_costume_special_act_active_skill USING masterdatabase (
  costume_id integer,
  skill_act_index integer,
  costume_special_act_active_skill_condition_type integer,
  costume_special_act_active_skill_condition_id integer
);

CREATE VIRTUAL TABLE m_costume_special_act_active_skill_condition_attribute USING masterdatabase (
  costume_special_act_active_skill_condition_id integer,
  costume_special_act_active_skill_condition_attribute_type integer
);

CREATE VIRTUAL TABLE m_costume_status_calculation USING masterdatabase (
  costume_status_calculation_id integer,
  hp_numerical_function_id integer,
  attack_numerical_function_id integer,
  vitality_numerical_function_id integer,
  agility_numerical_function_id integer,
  critical_ratio_permil_numerical_function_id integer,
  critical_attack_ratio_permil_numerical_function_id integer,
  evasion_ratio_permil_numerical_function_id integer
);

CREATE VIRTUAL TABLE m_deck_entrust_coefficient_attribute USING masterdatabase (
  entrust_attribute_type integer,
  attribute_type integer,
  coefficient_permil integer
);

CREATE VIRTUAL TABLE m_deck_entrust_coefficient_parts_series_bonus_count USING masterdatabase (
  parts_series_bonus_count integer,
  coefficient_permil integer
);

CREATE VIRTUAL TABLE m_deck_entrust_coefficient_status USING masterdatabase (
  entrust_deck_status_type integer,
  deck_status_type integer,
  coefficient_permil integer
);

CREATE VIRTUAL TABLE m_dokan USING masterdatabase (
  dokan_id integer,
  sort_order integer,
  dokan_type integer,
  start_datetime timestamp,
  end_datetime timestamp,
  dokan_content_group_id integer,
  target_user_status_type integer,
  unlock_main_function_type integer
);

CREATE VIRTUAL TABLE m_dokan_content_group USING masterdatabase (
  dokan_content_group_id integer,
  content_index integer,
  image_id integer,
  movie_id integer,
  dokan_text_id integer
);

CREATE VIRTUAL TABLE m_dokan_text USING masterdatabase (
  dokan_text_id integer,
  language_type integer,
  text text
);

CREATE VIRTUAL TABLE m_enhance_campaign USING masterdatabase (
  enhance_campaign_id integer,
  enhance_campaign_target_group_id integer,
  enhance_campaign_effect_type integer,
  enhance_campaign_effect_value integer,
  start_datetime timestamp,
  end_datetime timestamp,
  target_user_status_type integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_enhance_campaign_target_group USING masterdatabase (
  enhance_campaign_target_group_id integer,
  enhance_campaign_target_index integer,
  enhance_campaign_target_type integer,
  enhance_campaign_target_value integer
);

CREATE VIRTUAL TABLE m_evaluate_condition USING masterdatabase (
  evaluate_condition_id integer,
  evaluate_condition_function_type integer,
  evaluate_condition_evaluate_type integer,
  evaluate_condition_value_group_id integer,
  name_evaluate_condition_text_id integer
);

CREATE VIRTUAL TABLE m_evaluate_condition_value_group USING masterdatabase (
  evaluate_condition_value_group_id integer,
  group_index integer,
  value bigint
);

CREATE VIRTUAL TABLE m_event_quest_chapter USING masterdatabase (
  event_quest_chapter_id integer,
  event_quest_type integer,
  sort_order integer,
  name_event_quest_text_id integer,
  banner_asset_id integer,
  event_quest_link_id integer,
  event_quest_display_item_group_id integer,
  event_quest_sequence_group_id integer,
  start_datetime timestamp,
  end_datetime timestamp,
  display_sort_order integer
);

CREATE VIRTUAL TABLE m_event_quest_chapter_character USING masterdatabase (
  event_quest_chapter_id integer,
  character_id integer
);

CREATE VIRTUAL TABLE m_event_quest_chapter_difficulty_limit_content_unlock USING masterdatabase (
  event_quest_chapter_id integer,
  difficulty_type integer,
  unlock_evaluate_condition_id integer
);

CREATE VIRTUAL TABLE m_event_quest_chapter_limit_content_relation USING masterdatabase (
  event_quest_chapter_id integer,
  event_quest_limit_content_id integer
);

CREATE VIRTUAL TABLE m_event_quest_daily_group USING masterdatabase (
  event_quest_daily_group_id integer,
  start_datetime timestamp,
  end_datetime timestamp,
  event_quest_daily_group_target_chapter_id integer,
  event_quest_daily_group_complete_reward_id integer,
  event_quest_daily_group_message_id integer
);

CREATE VIRTUAL TABLE m_event_quest_daily_group_complete_reward USING masterdatabase (
  event_quest_daily_group_complete_reward_id integer,
  sort_order integer,
  possession_type integer,
  possession_id integer,
  count integer
);

CREATE VIRTUAL TABLE m_event_quest_daily_group_message USING masterdatabase (
  event_quest_daily_group_message_id integer,
  odds_number integer,
  weight integer,
  before_clear_message_text_id integer,
  after_clear_message_text_id integer
);

CREATE VIRTUAL TABLE m_event_quest_daily_group_target_chapter USING masterdatabase (
  event_quest_daily_group_target_chapter_id integer,
  sort_order integer,
  event_quest_chapter_id integer
);

CREATE VIRTUAL TABLE m_event_quest_display_item_group USING masterdatabase (
  event_quest_display_item_group_id integer,
  sort_order integer,
  possession_type integer,
  possession_id integer
);

CREATE VIRTUAL TABLE m_event_quest_guerrilla_free_open USING masterdatabase (
  event_quest_guerrilla_free_open_id integer,
  open_minutes integer,
  daily_open_max_count integer,
  start_datetime timestamp,
  end_datetime timestamp
);

CREATE VIRTUAL TABLE m_event_quest_guerrilla_free_open_schedule_correspondence USING masterdatabase (
  quest_id integer,
  quest_schedule_id integer
);

CREATE VIRTUAL TABLE m_event_quest_labyrinth_mob USING masterdatabase (
  event_quest_labyrinth_mob_id integer,
  mob_asset_id integer,
  before_stage_clear_text_asset_id integer,
  after_stage_clear_text_asset_id integer
);

CREATE VIRTUAL TABLE m_event_quest_labyrinth_quest_display USING masterdatabase (
  quest_id integer,
  bg_asset_id integer,
  mob_id integer
);

CREATE VIRTUAL TABLE m_event_quest_labyrinth_quest_effect_description_ability USING masterdatabase (
  event_quest_labyrinth_quest_effect_description_id integer,
  ability_id integer,
  ability_level integer
);

CREATE VIRTUAL TABLE m_event_quest_labyrinth_quest_effect_description_free USING masterdatabase (
  event_quest_labyrinth_quest_effect_description_id integer,
  asset_id integer
);

CREATE VIRTUAL TABLE m_event_quest_labyrinth_quest_effect_display USING masterdatabase (
  quest_id integer,
  sort_order integer,
  labyrinth_quest_effect_description_type integer,
  event_quest_labyrinth_quest_effect_description_id integer,
  effect_target_weapon_attribute_type integer
);

CREATE VIRTUAL TABLE m_event_quest_labyrinth_reward_group USING masterdatabase (
  event_quest_labyrinth_reward_group_id integer,
  sort_order integer,
  possession_type integer,
  possession_id integer,
  count integer
);

CREATE VIRTUAL TABLE m_event_quest_labyrinth_season USING masterdatabase (
  event_quest_chapter_id integer,
  season_number integer,
  start_datetime timestamp,
  end_datetime timestamp,
  season_reward_group_id integer
);

CREATE VIRTUAL TABLE m_event_quest_labyrinth_season_reward_group USING masterdatabase (
  event_quest_labyrinth_season_reward_group_id integer,
  head_quest_id integer,
  event_quest_labyrinth_reward_group_id integer
);

CREATE VIRTUAL TABLE m_event_quest_labyrinth_stage USING masterdatabase (
  event_quest_chapter_id integer,
  stage_order integer,
  start_sequence_sort_order integer,
  end_sequence_sort_order integer,
  stage_clear_reward_group_id integer,
  stage_accumulation_reward_group_id integer,
  mob1_id integer,
  mob2_id integer,
  treasure_asset_id integer
);

CREATE VIRTUAL TABLE m_event_quest_labyrinth_stage_accumulation_reward_group USING masterdatabase (
  event_quest_labyrinth_stage_accumulation_reward_group_id integer,
  quest_mission_clear_count integer,
  event_quest_labyrinth_reward_group_id integer
);

CREATE VIRTUAL TABLE m_event_quest_limit_content USING masterdatabase (
  event_quest_limit_content_id integer,
  costume_id integer,
  unlock_evaluate_condition_id integer,
  sort_order integer,
  deck_group_number integer,
  start_datetime timestamp,
  end_datetime timestamp,
  event_quest_limit_content_deck_restriction_id integer
);

CREATE VIRTUAL TABLE m_event_quest_limit_content_deck_restriction USING masterdatabase (
  event_quest_limit_content_deck_restriction_id integer,
  group_index integer,
  event_quest_limit_content_deck_restriction_target_id integer,
  start_datetime timestamp,
  end_datetime timestamp
);

CREATE VIRTUAL TABLE m_event_quest_limit_content_deck_restriction_target USING masterdatabase (
  event_quest_limit_content_deck_restriction_target_id integer,
  limit_content_deck_restriction_type integer
);

CREATE VIRTUAL TABLE m_event_quest_link USING masterdatabase (
  event_quest_link_id integer,
  destination_domain_type integer,
  destination_domain_id integer,
  possession_type integer,
  possession_id integer
);

CREATE VIRTUAL TABLE m_event_quest_sequence USING masterdatabase (
  event_quest_sequence_id integer,
  sort_order integer,
  quest_id integer
);

CREATE VIRTUAL TABLE m_event_quest_sequence_group USING masterdatabase (
  event_quest_sequence_group_id integer,
  difficulty_type integer,
  event_quest_sequence_id integer
);

CREATE VIRTUAL TABLE m_event_quest_tower_accumulation_reward USING masterdatabase (
  event_quest_chapter_id integer,
  event_quest_tower_accumulation_reward_group_id integer
);

CREATE VIRTUAL TABLE m_event_quest_tower_accumulation_reward_group USING masterdatabase (
  event_quest_tower_accumulation_reward_group_id integer,
  quest_mission_clear_count integer,
  event_quest_tower_reward_group_id integer
);

CREATE VIRTUAL TABLE m_event_quest_tower_asset USING masterdatabase (
  event_quest_chapter_id integer,
  asset_id integer
);

CREATE VIRTUAL TABLE m_event_quest_tower_reward_group USING masterdatabase (
  event_quest_tower_reward_group_id integer,
  sort_order integer,
  possession_type integer,
  possession_id integer,
  count integer
);

CREATE VIRTUAL TABLE m_event_quest_unlock_condition USING masterdatabase (
  event_quest_type integer,
  character_id integer,
  quest_id integer,
  unlock_condition_type integer,
  condition_value integer,
  unlock_evaluate_condition_id integer
);

CREATE VIRTUAL TABLE m_explore USING masterdatabase (
  explore_id integer,
  explore_unlock_condition_id integer,
  start_datetime timestamp,
  consume_item_count integer,
  reward_lottery_count integer
);

CREATE VIRTUAL TABLE m_explore_grade_asset USING masterdatabase (
  explore_grade_id integer,
  asset_grade_icon_id integer
);

CREATE VIRTUAL TABLE m_explore_grade_score USING masterdatabase (
  explore_id integer,
  necessary_score integer,
  explore_grade_id integer
);

CREATE VIRTUAL TABLE m_explore_group USING masterdatabase (
  explore_group_id integer,
  difficulty_type integer,
  explore_id integer
);

CREATE VIRTUAL TABLE m_explore_unlock_condition USING masterdatabase (
  explore_unlock_condition_id integer,
  explore_unlock_condition_type integer,
  condition_value integer
);

CREATE VIRTUAL TABLE m_extra_quest_group USING masterdatabase (
  quest_id integer,
  extra_quest_index integer,
  extra_quest_id integer
);

CREATE VIRTUAL TABLE m_extra_quest_group_in_main_quest_chapter USING masterdatabase (
  main_quest_chapter_id integer,
  extra_quest_index integer,
  extra_quest_id integer
);

CREATE VIRTUAL TABLE m_field_effect_bless_relation USING masterdatabase (
  field_effect_group_id integer,
  field_effect_bless_relation_index integer,
  weapon_id integer
);

CREATE VIRTUAL TABLE m_field_effect_decrease_point USING masterdatabase (
  weapon_id integer,
  field_effect_ability_id integer,
  decrease_point integer
);

CREATE VIRTUAL TABLE m_field_effect_group USING masterdatabase (
  field_effect_group_id integer,
  field_effect_group_index integer,
  ability_id integer,
  default_ability_level integer,
  field_effect_apply_scope_type integer,
  field_effect_asset_id integer
);

CREATE VIRTUAL TABLE m_gacha_medal USING masterdatabase (
  gacha_medal_id integer,
  ceiling_count integer,
  consumable_item_id integer,
  shop_transition_gacha_id integer,
  auto_convert_datetime timestamp,
  conversion_rate integer
);

CREATE VIRTUAL TABLE m_gift_text USING masterdatabase (
  gift_text_id integer,
  language_type integer,
  text text
);

CREATE VIRTUAL TABLE m_gimmick USING masterdatabase (
  gimmick_id integer,
  gimmick_type integer,
  gimmick_ornament_group_id integer,
  clear_evaluate_condition_id integer,
  release_evaluate_condition_id integer
);

CREATE VIRTUAL TABLE m_gimmick_additional_asset USING masterdatabase (
  gimmick_id integer,
  gimmick_texture_path text
);

CREATE VIRTUAL TABLE m_gimmick_extra_quest USING masterdatabase (
  gimmick_id integer,
  gimmick_ornament_index integer,
  extra_quest_id integer
);

CREATE VIRTUAL TABLE m_gimmick_group USING masterdatabase (
  gimmick_group_id integer,
  group_index integer,
  gimmick_id integer
);

CREATE VIRTUAL TABLE m_gimmick_group_event_log USING masterdatabase (
  gimmick_group_id integer,
  event_log_text_id integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_gimmick_interval USING masterdatabase (
  gimmick_id integer,
  initial_value integer,
  max_value integer,
  interval_value integer
);

CREATE VIRTUAL TABLE m_gimmick_ornament USING masterdatabase (
  gimmick_ornament_group_id integer,
  gimmick_ornament_index integer,
  gimmick_ornament_view_id integer,
  count integer,
  chapter_id integer,
  position_x real,
  position_y real,
  position_z real,
  rotation real,
  scale real,
  sort_order integer,
  asset_background_id integer,
  icon_difficulty_value integer,
  rotation_x real,
  rotation_z real
);

CREATE VIRTUAL TABLE m_gimmick_sequence USING masterdatabase (
  gimmick_sequence_id integer,
  gimmick_sequence_clear_condition_type integer,
  next_gimmick_sequence_group_id integer,
  gimmick_group_id integer,
  gimmick_sequence_reward_group_id integer,
  flow_type integer,
  progress_require_hour integer,
  progress_start_datetime timestamp
);

CREATE VIRTUAL TABLE m_gimmick_sequence_group USING masterdatabase (
  gimmick_sequence_group_id integer,
  group_index integer,
  gimmick_sequence_id integer
);

CREATE VIRTUAL TABLE m_gimmick_sequence_reward_group USING masterdatabase (
  gimmick_sequence_reward_group_id integer,
  group_index integer,
  possession_type integer,
  possession_id integer,
  count integer
);

CREATE VIRTUAL TABLE m_gimmick_sequence_schedule USING masterdatabase (
  gimmick_sequence_schedule_id integer,
  start_datetime timestamp,
  end_datetime timestamp,
  first_gimmick_sequence_id integer,
  release_evaluate_condition_id integer
);

CREATE VIRTUAL TABLE m_headup_display_view USING masterdatabase (
  headup_display_view_id integer,
  view_skill_button_type integer,
  hp_bar_display_type integer,
  view_name_text_type integer,
  view_buff_abnormal_type integer,
  view_level_text_type integer
);

CREATE VIRTUAL TABLE m_help USING masterdatabase (
  help_type integer,
  help_item_id integer,
  help_page_group_id integer
);

CREATE VIRTUAL TABLE m_help_category USING masterdatabase (
  help_category_id integer,
  sort_order integer,
  title_text_asset_id integer,
  is_hidden_on_list boolean
);

CREATE VIRTUAL TABLE m_help_item USING masterdatabase (
  help_item_id integer,
  help_category_id integer,
  sort_order integer,
  total_page_count integer,
  title_text_asset_id integer,
  asset_name text,
  is_hidden_on_list boolean
);

CREATE VIRTUAL TABLE m_help_page_group USING masterdatabase (
  help_page_group_id integer,
  sort_order integer,
  help_page_id integer
);

CREATE VIRTUAL TABLE m_important_item USING masterdatabase (
  important_item_id integer,
  name_important_item_text_id integer,
  description_important_item_text_id integer,
  sort_order integer,
  asset_category_id integer,
  asset_variation_id integer,
  important_item_effect_id integer,
  report_id integer,
  cage_memory_id integer,
  important_item_type integer,
  external_reference_id integer
);

CREATE VIRTUAL TABLE m_important_item_effect USING masterdatabase (
  important_item_effect_id integer,
  important_item_effect_grouping_id integer,
  priority integer,
  important_item_effect_type integer,
  important_item_effect_target_id integer,
  start_datetime timestamp,
  end_datetime timestamp
);

CREATE VIRTUAL TABLE m_important_item_effect_drop_count USING masterdatabase (
  important_item_effect_drop_count_id integer,
  count_permil integer,
  important_item_effect_target_quest_group_id integer,
  important_item_effect_target_item_group_id integer
);

CREATE VIRTUAL TABLE m_important_item_effect_drop_rate USING masterdatabase (
  important_item_effect_drop_rate_id integer,
  rate_permil integer,
  important_item_effect_target_quest_group_id integer,
  important_item_effect_target_item_group_id integer
);

CREATE VIRTUAL TABLE m_important_item_effect_target_item_group USING masterdatabase (
  important_item_effect_target_item_group_id integer,
  target_index integer,
  possession_type integer,
  possession_id integer
);

CREATE VIRTUAL TABLE m_important_item_effect_target_quest_group USING masterdatabase (
  important_item_effect_target_quest_group_id integer,
  target_index integer,
  important_item_effect_target_quest_group_type integer,
  target_value integer
);

CREATE VIRTUAL TABLE m_important_item_effect_unlock_function USING masterdatabase (
  important_item_effect_unlock_function_id integer,
  important_item_effect_unlock_function_type integer,
  unlock_function_effect_value integer
);

CREATE VIRTUAL TABLE m_library_event_quest_story_grouping USING masterdatabase (
  library_story_grouping_id integer,
  event_quest_chapter_id integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_library_main_quest_group USING masterdatabase (
  library_main_quest_group_id integer,
  main_quest_chapter_id integer,
  sort_order integer,
  chapter_text_asset_id integer,
  first_still_asset_order integer,
  second_still_asset_order integer
);

CREATE VIRTUAL TABLE m_library_main_quest_story USING masterdatabase (
  library_main_quest_group_id integer,
  sort_order integer,
  recollection_scene_id integer,
  library_main_quest_story_unlock_evaluate_condition_id integer,
  text_asset_id integer
);

CREATE VIRTUAL TABLE m_library_movie USING masterdatabase (
  library_movie_id integer,
  title_library_text_id integer,
  library_movie_category_id integer,
  sort_order integer,
  library_movie_unlock_condition_id integer,
  library_movie_unlock_evaluate_condition_id integer,
  movie_id integer
);

CREATE VIRTUAL TABLE m_library_movie_category USING masterdatabase (
  library_movie_category_id integer,
  name_library_text_id integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_library_movie_unlock_condition USING masterdatabase (
  library_movie_unlock_condition_id integer,
  unlock_condition_type integer,
  condition_value integer
);

CREATE VIRTUAL TABLE m_library_record_grouping USING masterdatabase (
  library_record_type integer,
  sort_order integer,
  library_record_asset_id integer
);

CREATE VIRTUAL TABLE m_limited_open_text USING masterdatabase (
  limited_open_target_type integer,
  target_id integer,
  open_achievement_text_asset_id integer,
  local_push_text_asset_id integer,
  open_achievement_text_group_id integer
);

CREATE VIRTUAL TABLE m_limited_open_text_group USING masterdatabase (
  limited_open_text_group_id integer,
  sort_order integer,
  limited_open_text_display_condition_type integer,
  limited_open_text_display_condition_value integer,
  text_asset_id integer
);

CREATE VIRTUAL TABLE m_list_setting_ability_group USING masterdatabase (
  list_setting_ability_group_id integer,
  sort_order integer,
  list_setting_ability_group_target_id integer,
  asset_id integer,
  list_setting_ability_group_type integer,
  list_setting_display_start_datetime timestamp
);

CREATE VIRTUAL TABLE m_list_setting_ability_group_target USING masterdatabase (
  list_setting_ability_group_target_id integer,
  sort_order integer,
  ability_id integer
);

CREATE VIRTUAL TABLE m_login_bonus USING masterdatabase (
  login_bonus_id integer,
  sort_order integer,
  login_bonus_start_condition_id integer,
  total_page_count integer,
  start_datetime timestamp,
  end_datetime timestamp,
  stamp_receive_end_datetime timestamp,
  login_bonus_asset_name text
);

CREATE VIRTUAL TABLE m_login_bonus_stamp USING masterdatabase (
  login_bonus_id integer,
  lower_page_number integer,
  stamp_number integer,
  reward_possession_type integer,
  reward_possession_id integer,
  reward_count integer
);

CREATE VIRTUAL TABLE m_main_quest_chapter USING masterdatabase (
  main_quest_chapter_id integer,
  main_quest_route_id integer,
  sort_order integer,
  main_quest_sequence_group_id integer,
  portal_cage_character_group_id integer,
  start_datetime timestamp
);

CREATE VIRTUAL TABLE m_main_quest_portal_cage_character USING masterdatabase (
  quest_scene_id integer,
  portal_cage_character_group_id integer
);

CREATE VIRTUAL TABLE m_main_quest_route USING masterdatabase (
  main_quest_route_id integer,
  main_quest_season_id integer,
  sort_order integer,
  character_id integer
);

CREATE VIRTUAL TABLE m_main_quest_route_another_replay_flow_unlock_condition USING masterdatabase (
  main_quest_route_id integer,
  unlock_evaluate_condition_id integer,
  unlock_target_main_quest_route_id integer
);

CREATE VIRTUAL TABLE m_main_quest_season USING masterdatabase (
  main_quest_season_id integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_main_quest_sequence USING masterdatabase (
  main_quest_sequence_id integer,
  sort_order integer,
  quest_id integer
);

CREATE VIRTUAL TABLE m_main_quest_sequence_group USING masterdatabase (
  main_quest_sequence_group_id integer,
  difficulty_type integer,
  main_quest_sequence_id integer
);

CREATE VIRTUAL TABLE m_maintenance USING masterdatabase (
  maintenance_id integer,
  start_datetime timestamp,
  end_datetime timestamp,
  maintenance_group_id integer
);

CREATE VIRTUAL TABLE m_maintenance_group USING masterdatabase (
  maintenance_group_id integer,
  api_path text,
  priority integer,
  screen_transition_type integer,
  block_function_type integer,
  block_function_value text
);

CREATE VIRTUAL TABLE m_material USING masterdatabase (
  material_id integer,
  material_type integer,
  rarity_type integer,
  weapon_type integer,
  attribute_type integer,
  effect_value integer,
  sell_price integer,
  asset_name text,
  asset_category_id integer,
  asset_variation_id integer,
  material_sale_obtain_possession_id integer
);

CREATE VIRTUAL TABLE m_material_sale_obtain_possession USING masterdatabase (
  material_sale_obtain_possession_id integer,
  sort_order integer,
  possession_type integer,
  possession_id integer,
  count integer
);

CREATE VIRTUAL TABLE m_mission USING masterdatabase (
  mission_id integer,
  mission_group_id integer,
  sort_order_in_mission_group integer,
  mission_unlock_condition_id integer,
  is_not_show_before_clear boolean,
  name_mission_text_id integer,
  mission_link_id integer,
  mission_clear_condition_type integer,
  mission_clear_condition_group_id integer,
  clear_condition_value integer,
  mission_clear_condition_option_group_id integer,
  mission_reward_id integer,
  mission_term_id integer,
  min_expiration_days integer,
  related_main_function_type integer,
  mission_clear_condition_option_detail_group_id integer,
  mission_unlock_condition_detail_group_id integer
);

CREATE VIRTUAL TABLE m_mission_clear_condition_value_view USING masterdatabase (
  mission_clear_condition_type integer,
  view_clear_condition_value integer
);

CREATE VIRTUAL TABLE m_mission_group USING masterdatabase (
  mission_group_id integer,
  mission_category_type integer,
  label_mission_text_id integer,
  sort_order_in_label integer,
  asset_id integer,
  mission_group_unlock_condition_group_id integer,
  mission_sub_category_id integer
);

CREATE VIRTUAL TABLE m_mission_link USING masterdatabase (
  mission_link_id integer,
  destination_domain_type integer,
  destination_domain_id integer
);

CREATE VIRTUAL TABLE m_mission_pass USING masterdatabase (
  mission_pass_id integer,
  start_datetime timestamp,
  end_datetime timestamp,
  premium_item_id integer,
  mission_pass_level_group_id integer,
  mission_pass_reward_group_id integer
);

CREATE VIRTUAL TABLE m_mission_pass_level_group USING masterdatabase (
  mission_pass_level_group_id integer,
  level integer,
  necessary_point integer
);

CREATE VIRTUAL TABLE m_mission_pass_mission_group USING masterdatabase (
  mission_pass_id integer,
  mission_group_id integer
);

CREATE VIRTUAL TABLE m_mission_pass_reward_group USING masterdatabase (
  mission_pass_reward_group_id integer,
  level integer,
  is_premium boolean,
  sort_order integer,
  possession_type integer,
  possession_id integer,
  count integer
);

CREATE VIRTUAL TABLE m_mission_reward USING masterdatabase (
  mission_reward_id integer,
  possession_type integer,
  possession_id integer,
  count integer
);

CREATE VIRTUAL TABLE m_mission_sub_category_text USING masterdatabase (
  mission_sub_category_id integer,
  text_id integer
);

CREATE VIRTUAL TABLE m_mission_term USING masterdatabase (
  mission_term_id integer,
  start_datetime timestamp,
  end_datetime timestamp
);

CREATE VIRTUAL TABLE m_mission_unlock_condition USING masterdatabase (
  mission_unlock_condition_id integer,
  mission_unlock_condition_type integer,
  condition_value integer
);

CREATE VIRTUAL TABLE m_mom_banner USING masterdatabase (
  mom_banner_id integer,
  sort_order_desc integer,
  destination_domain_type integer,
  destination_domain_id integer,
  banner_asset_name text,
  is_emphasis boolean,
  start_datetime timestamp,
  end_datetime timestamp,
  target_user_status_type integer
);

CREATE VIRTUAL TABLE m_mom_point_banner USING masterdatabase (
  mom_point_banner_id integer,
  banner_asset_id integer,
  destination_information_id integer,
  start_datetime timestamp,
  end_datetime timestamp
);

CREATE VIRTUAL TABLE m_movie USING masterdatabase (
  movie_id integer,
  asset_id integer
);

CREATE VIRTUAL TABLE m_navi_cut_in USING masterdatabase (
  navi_cut_in_id integer,
  related_cut_in_function_type integer,
  sort_order integer,
  start_datetime timestamp,
  end_datetime timestamp,
  navi_cut_in_content_group_id integer,
  related_cut_in_function_value integer
);

CREATE VIRTUAL TABLE m_navi_cut_in_content_group USING masterdatabase (
  navi_cut_in_content_group_id integer,
  content_index integer,
  navi_cut_in_text_id integer
);

CREATE VIRTUAL TABLE m_navi_cut_in_text USING masterdatabase (
  navi_cut_in_text_id integer,
  language_type integer,
  text text
);

CREATE VIRTUAL TABLE m_numerical_function USING masterdatabase (
  numerical_function_id integer,
  numerical_function_type integer,
  numerical_function_parameter_group_id integer
);

CREATE VIRTUAL TABLE m_numerical_function_parameter_group USING masterdatabase (
  numerical_function_parameter_group_id integer,
  parameter_index integer,
  parameter_value integer
);

CREATE VIRTUAL TABLE m_numerical_parameter_map USING masterdatabase (
  numerical_parameter_map_id integer,
  parameter_key integer,
  parameter_value integer
);

CREATE VIRTUAL TABLE m_omikuji USING masterdatabase (
  omikuji_id integer,
  start_datetime timestamp,
  end_datetime timestamp,
  omikuji_asset_id integer
);

CREATE VIRTUAL TABLE m_override_hit_effect_condition_critical USING masterdatabase (
  override_hit_effect_condition_id integer,
  is_critical boolean
);

CREATE VIRTUAL TABLE m_override_hit_effect_condition_damage_attribute USING masterdatabase (
  override_hit_effect_condition_id integer,
  is_excepting boolean,
  attribute_type integer
);

CREATE VIRTUAL TABLE m_override_hit_effect_condition_group USING masterdatabase (
  override_hit_effect_condition_group_id integer,
  condition_index integer,
  condition_type integer,
  override_hit_effect_condition_id integer
);

CREATE VIRTUAL TABLE m_override_hit_effect_condition_skill_executor USING masterdatabase (
  override_hit_effect_condition_id integer,
  skill_owner_category_type integer
);

CREATE VIRTUAL TABLE m_parts USING masterdatabase (
  parts_id integer,
  rarity_type integer,
  parts_group_id integer,
  parts_status_main_lottery_group_id integer,
  parts_status_sub_lottery_group_id integer,
  parts_initial_lottery_id integer
);

CREATE VIRTUAL TABLE m_parts_enhanced USING masterdatabase (
  parts_enhanced_id integer,
  parts_id integer,
  parts_status_main_id integer,
  level integer,
  is_random_sub_status_count boolean,
  sub_status_count integer
);

CREATE VIRTUAL TABLE m_parts_enhanced_sub_status USING masterdatabase (
  parts_enhanced_id integer,
  status_index integer,
  parts_status_sub_lottery_id integer,
  level integer,
  status_kind_type integer,
  status_calculation_type integer,
  fixed_status_change_value integer
);

CREATE VIRTUAL TABLE m_parts_group USING masterdatabase (
  parts_group_id integer,
  parts_series_id integer,
  sort_order integer,
  parts_group_asset_id integer
);

CREATE VIRTUAL TABLE m_parts_level_up_price_group USING masterdatabase (
  parts_level_up_price_group_id integer,
  level_lower_limit integer,
  gold integer
);

CREATE VIRTUAL TABLE m_parts_level_up_rate_group USING masterdatabase (
  parts_level_up_rate_group_id integer,
  level_lower_limit integer,
  success_rate_permil integer
);

CREATE VIRTUAL TABLE m_parts_rarity USING masterdatabase (
  rarity_type integer,
  parts_level_up_rate_group_id integer,
  parts_level_up_price_group_id integer,
  sell_price_numerical_function_id integer
);

CREATE VIRTUAL TABLE m_parts_series USING masterdatabase (
  parts_series_id integer,
  parts_series_bonus_ability_group_id integer,
  parts_series_asset_id integer,
  list_setting_display_start_datetime timestamp
);

CREATE VIRTUAL TABLE m_parts_series_bonus_ability_group USING masterdatabase (
  parts_series_bonus_ability_group_id integer,
  set_count integer,
  ability_id integer,
  ability_level integer
);

CREATE VIRTUAL TABLE m_parts_status_main USING masterdatabase (
  parts_status_main_id integer,
  status_kind_type integer,
  status_calculation_type integer,
  status_change_initial_value integer,
  status_numerical_function_id integer
);

CREATE VIRTUAL TABLE m_platform_payment USING masterdatabase (
  platform_payment_id integer,
  platform_type integer,
  product_id_suffix text
);

CREATE VIRTUAL TABLE m_platform_payment_price USING masterdatabase (
  platform_payment_id integer,
  platform_type integer,
  currency_type integer,
  price decimal
);

CREATE VIRTUAL TABLE m_portal_cage_access_point_function_group USING masterdatabase (
  access_point_function_group_id integer,
  access_point_function_index integer,
  access_point_function_id integer
);

CREATE VIRTUAL TABLE m_portal_cage_access_point_function_group_schedule USING masterdatabase (
  portal_cage_access_point_function_group_schedule_id integer,
  priority_desc integer,
  access_point_type integer,
  access_point_function_group_id integer,
  start_datetime timestamp,
  end_datetime timestamp
);

CREATE VIRTUAL TABLE m_portal_cage_character_group USING masterdatabase (
  portal_cage_character_group_id integer,
  player_character_actor_object_id integer,
  navi_character_actor_object_id integer,
  navi_menu_actor_object_id integer,
  tutorial_type integer
);

CREATE VIRTUAL TABLE m_portal_cage_gate USING masterdatabase (
  portal_cage_gate_id integer,
  gate_position_index integer,
  portal_cage_access_point_function_group_schedule_id integer
);

CREATE VIRTUAL TABLE m_portal_cage_scene USING masterdatabase (
  portal_cage_scene_id integer,
  portal_cage_character_group_id integer,
  portal_cage_drop_id integer,
  portal_cage_gate_id integer
);

CREATE VIRTUAL TABLE m_possession_acquisition_route USING masterdatabase (
  possession_type integer,
  possession_id integer,
  sort_order integer,
  acquisition_route_type integer,
  route_id integer,
  relation_value text,
  start_datetime timestamp,
  end_datetime timestamp
);

CREATE VIRTUAL TABLE m_power_calculation_constant_value USING masterdatabase (
  power_calculation_constant_value_type integer,
  constant_value integer
);

CREATE VIRTUAL TABLE m_power_reference_status_group USING masterdatabase (
  power_reference_status_group_id integer,
  reference_status_type integer,
  attribute_condition_type integer,
  coefficient_value_permil integer
);

CREATE VIRTUAL TABLE m_premium_item USING masterdatabase (
  premium_item_id integer,
  premium_item_type integer,
  start_datetime timestamp,
  end_datetime timestamp
);

CREATE VIRTUAL TABLE m_pvp_background USING masterdatabase (
  asset_background_id integer,
  battle_field_locale_setting_index integer,
  battle_point_index integer,
  random_weight integer,
  post_process_configuration_index integer,
  battle_camera_controller_asset_id integer,
  battle_start_camera_type integer,
  wave_start_act_asset_id integer,
  wave_end_act_asset_id integer
);

CREATE VIRTUAL TABLE m_pvp_grade USING masterdatabase (
  pvp_grade_id integer,
  necessary_pvp_point integer,
  icon_asset_id integer,
  pvp_grade_weekly_reward_group_id integer,
  pvp_grade_one_match_reward_group_id integer
);

CREATE VIRTUAL TABLE m_pvp_grade_group USING masterdatabase (
  pvp_grade_group_id integer,
  pvp_grade_id integer,
  necessary_pvp_point integer,
  icon_asset_id integer,
  pvp_grade_weekly_reward_group_id integer,
  pvp_grade_one_match_reward_group_id integer
);

CREATE VIRTUAL TABLE m_pvp_grade_one_match_reward USING masterdatabase (
  pvp_grade_one_match_reward_id integer,
  pvp_reward_id integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_pvp_grade_one_match_reward_group USING masterdatabase (
  pvp_grade_one_match_reward_group_id integer,
  pvp_grade_one_match_reward_id integer,
  weight integer
);

CREATE VIRTUAL TABLE m_pvp_grade_weekly_reward_group USING masterdatabase (
  pvp_grade_weekly_reward_group_id integer,
  pvp_reward_id integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_pvp_reward USING masterdatabase (
  pvp_reward_id integer,
  possession_type integer,
  possession_id integer,
  count integer
);

CREATE VIRTUAL TABLE m_pvp_season USING masterdatabase (
  pvp_season_id integer,
  name_asset_path text,
  season_start_datetime timestamp,
  season_end_datetime timestamp,
  pvp_season_grouping_id integer,
  is_invalid boolean,
  pvp_weekly_rank_reward_rank_group_id integer,
  pvp_season_rank_reward_rank_group_id integer,
  pvp_grade_group_id integer,
  pvp_initial_point_addition_group_id integer,
  pvp_season_deck_power_threshold_grouping_id integer
);

CREATE VIRTUAL TABLE m_pvp_season_grade USING masterdatabase (
  pvp_grade_id integer,
  pvp_season_id integer,
  necessary_pvp_point integer,
  icon_asset_id integer,
  pvp_grade_weekly_reward_group_id integer,
  pvp_grade_one_match_reward_group_id integer
);

CREATE VIRTUAL TABLE m_pvp_season_grouping USING masterdatabase (
  pvp_season_grouping_id integer,
  group_id integer,
  divide_weight integer
);

CREATE VIRTUAL TABLE m_pvp_season_rank_reward USING masterdatabase (
  rank_lower_limit integer,
  pvp_season_rank_reward_group_id integer
);

CREATE VIRTUAL TABLE m_pvp_season_rank_reward_group USING masterdatabase (
  pvp_season_rank_reward_group_id integer,
  pvp_reward_id integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_pvp_season_rank_reward_per_season USING masterdatabase (
  rank_lower_limit integer,
  pvp_season_id integer,
  pvp_season_rank_reward_group_id integer
);

CREATE VIRTUAL TABLE m_pvp_season_rank_reward_rank_group USING masterdatabase (
  pvp_season_rank_reward_rank_group_id integer,
  rank_lower_limit integer,
  pvp_season_rank_reward_group_id integer
);

CREATE VIRTUAL TABLE m_pvp_weekly_rank_reward_group USING masterdatabase (
  pvp_weekly_rank_reward_group_id integer,
  pvp_reward_id integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_pvp_weekly_rank_reward_rank_group USING masterdatabase (
  pvp_weekly_rank_reward_rank_group_id integer,
  rank_lower_limit integer,
  pvp_weekly_rank_reward_group_id integer
);

CREATE VIRTUAL TABLE m_pvp_win_streak_count_effect USING masterdatabase (
  win_streak_count integer,
  ability_id integer,
  ability_level integer
);

CREATE VIRTUAL TABLE m_quest USING masterdatabase (
  quest_id integer,
  name_quest_text_id integer,
  picture_book_name_quest_text_id integer,
  quest_release_condition_list_id integer,
  story_quest_text_id integer,
  quest_display_attribute_group_id integer,
  recommended_deck_power integer,
  quest_first_clear_reward_group_id integer,
  quest_pickup_reward_group_id integer,
  quest_deck_restriction_group_id integer,
  quest_mission_group_id integer,
  stamina integer,
  user_exp integer,
  character_exp integer,
  costume_exp integer,
  gold integer,
  daily_clearable_count integer,
  is_run_in_the_background boolean,
  is_counted_as_quest boolean,
  quest_bonus_id integer,
  is_not_show_after_clear boolean,
  is_big_win_target boolean,
  is_usable_skip_ticket boolean,
  quest_replay_flow_reward_group_id integer,
  invisible_quest_mission_group_id integer,
  field_effect_group_id integer
);

CREATE VIRTUAL TABLE m_quest_bonus USING masterdatabase (
  quest_bonus_id integer,
  quest_bonus_character_group_id integer,
  quest_bonus_costume_group_id integer,
  quest_bonus_weapon_group_id integer,
  quest_bonus_costume_setting_group_id integer,
  quest_bonus_ally_character_id integer
);

CREATE VIRTUAL TABLE m_quest_bonus_ability USING masterdatabase (
  quest_bonus_effect_id integer,
  ability_id integer,
  level integer
);

CREATE VIRTUAL TABLE m_quest_bonus_ally_character USING masterdatabase (
  quest_bonus_ally_character_id integer,
  quest_bonus_effect_group_id integer,
  quest_bonus_term_group_id integer
);

CREATE VIRTUAL TABLE m_quest_bonus_character_group USING masterdatabase (
  quest_bonus_character_group_id integer,
  character_id integer,
  quest_bonus_effect_group_id integer,
  quest_bonus_term_group_id integer
);

CREATE VIRTUAL TABLE m_quest_bonus_costume_group USING masterdatabase (
  quest_bonus_costume_group_id integer,
  costume_id integer,
  quest_bonus_effect_group_id integer,
  quest_bonus_term_group_id integer
);

CREATE VIRTUAL TABLE m_quest_bonus_costume_setting_group USING masterdatabase (
  quest_bonus_costume_setting_group_id integer,
  costume_id integer,
  limit_break_count_lower_limit integer,
  quest_bonus_effect_group_id integer,
  quest_bonus_term_group_id integer
);

CREATE VIRTUAL TABLE m_quest_bonus_drop_reward USING masterdatabase (
  quest_bonus_effect_id integer,
  possession_type integer,
  possession_id integer,
  additional_count integer
);

CREATE VIRTUAL TABLE m_quest_bonus_effect_group USING masterdatabase (
  quest_bonus_effect_group_id integer,
  sort_order integer,
  quest_bonus_type integer,
  quest_bonus_effect_id integer
);

CREATE VIRTUAL TABLE m_quest_bonus_exp USING masterdatabase (
  quest_bonus_effect_id integer,
  exp_type integer,
  bonus_value_permil integer
);

CREATE VIRTUAL TABLE m_quest_bonus_term_group USING masterdatabase (
  quest_bonus_term_group_id integer,
  sort_order integer,
  start_datetime timestamp,
  end_datetime timestamp
);

CREATE VIRTUAL TABLE m_quest_bonus_weapon_group USING masterdatabase (
  quest_bonus_weapon_group_id integer,
  weapon_id integer,
  limit_break_count_lower_limit integer,
  quest_bonus_effect_group_id integer,
  quest_bonus_term_group_id integer
);

CREATE VIRTUAL TABLE m_quest_campaign USING masterdatabase (
  quest_campaign_id integer,
  quest_campaign_target_group_id integer,
  quest_campaign_effect_group_id integer,
  start_datetime timestamp,
  end_datetime timestamp,
  target_user_status_type integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_quest_campaign_effect_group USING masterdatabase (
  quest_campaign_effect_group_id integer,
  quest_campaign_effect_type integer,
  quest_campaign_effect_value integer,
  quest_campaign_target_item_group_id integer
);

CREATE VIRTUAL TABLE m_quest_campaign_target_group USING masterdatabase (
  quest_campaign_target_group_id integer,
  quest_campaign_target_index integer,
  quest_campaign_target_type integer,
  quest_campaign_target_value integer
);

CREATE VIRTUAL TABLE m_quest_campaign_target_item_group USING masterdatabase (
  quest_campaign_target_item_group_id integer,
  target_index integer,
  possession_type integer,
  possession_id integer,
  count integer
);

CREATE VIRTUAL TABLE m_quest_deck_restriction_group USING masterdatabase (
  quest_deck_restriction_group_id integer,
  slot_number integer,
  quest_deck_restriction_type integer,
  restriction_value integer
);

CREATE VIRTUAL TABLE m_quest_deck_restriction_group_unlock USING masterdatabase (
  quest_deck_restriction_group_id integer,
  unlock_evaluate_condition_id integer
);

CREATE VIRTUAL TABLE m_quest_display_attribute_group USING masterdatabase (
  quest_display_attribute_group_id integer,
  sort_order integer,
  quest_display_attribute_type integer,
  quest_display_attribute_icon_size_type integer
);

CREATE VIRTUAL TABLE m_quest_display_enemy_thumbnail_replace USING masterdatabase (
  quest_id integer,
  priority integer,
  replace_condition_type integer,
  replace_method_type integer,
  replace_value integer
);

CREATE VIRTUAL TABLE m_quest_first_clear_reward_group USING masterdatabase (
  quest_first_clear_reward_group_id integer,
  quest_first_clear_reward_type integer,
  sort_order integer,
  possession_type integer,
  possession_id integer,
  count integer,
  is_pickup boolean
);

CREATE VIRTUAL TABLE m_quest_mission USING masterdatabase (
  quest_mission_id integer,
  quest_mission_condition_type integer,
  condition_value integer,
  quest_mission_reward_id integer,
  quest_mission_condition_value_group_id integer
);

CREATE VIRTUAL TABLE m_quest_mission_condition_value_group USING masterdatabase (
  quest_mission_condition_value_group_id integer,
  sort_order integer,
  condition_value integer
);

CREATE VIRTUAL TABLE m_quest_mission_group USING masterdatabase (
  quest_mission_group_id integer,
  sort_order integer,
  quest_mission_id integer
);

CREATE VIRTUAL TABLE m_quest_mission_reward USING masterdatabase (
  quest_mission_reward_id integer,
  possession_type integer,
  possession_id integer,
  count integer
);

CREATE VIRTUAL TABLE m_quest_pickup_reward_group USING masterdatabase (
  quest_pickup_reward_group_id integer,
  sort_order integer,
  battle_drop_reward_id integer
);

CREATE VIRTUAL TABLE m_quest_relation_main_flow USING masterdatabase (
  main_flow_quest_id integer,
  difficulty_type integer,
  replay_flow_quest_id integer,
  sub_flow_quest_id integer
);

CREATE VIRTUAL TABLE m_quest_release_condition_big_hunt_score USING masterdatabase (
  quest_release_condition_id integer,
  big_hunt_boss_id integer,
  necessary_score bigint
);

CREATE VIRTUAL TABLE m_quest_release_condition_character_level USING masterdatabase (
  quest_release_condition_id integer,
  character_id integer,
  character_level integer
);

CREATE VIRTUAL TABLE m_quest_release_condition_deck_power USING masterdatabase (
  quest_release_condition_id integer,
  max_deck_power integer
);

CREATE VIRTUAL TABLE m_quest_release_condition_group USING masterdatabase (
  quest_release_condition_group_id integer,
  sort_order integer,
  quest_release_condition_type integer,
  quest_release_condition_id integer
);

CREATE VIRTUAL TABLE m_quest_release_condition_list USING masterdatabase (
  quest_release_condition_list_id integer,
  quest_release_condition_group_id integer,
  condition_operation_type integer
);

CREATE VIRTUAL TABLE m_quest_release_condition_quest_challenge USING masterdatabase (
  quest_release_condition_id integer,
  quest_id integer
);

CREATE VIRTUAL TABLE m_quest_release_condition_quest_clear USING masterdatabase (
  quest_release_condition_id integer,
  quest_id integer
);

CREATE VIRTUAL TABLE m_quest_release_condition_user_level USING masterdatabase (
  quest_release_condition_id integer,
  user_level integer
);

CREATE VIRTUAL TABLE m_quest_release_condition_weapon_acquisition USING masterdatabase (
  quest_release_condition_id integer,
  weapon_id integer
);

CREATE VIRTUAL TABLE m_quest_replay_flow_reward_group USING masterdatabase (
  quest_replay_flow_reward_group_id integer,
  sort_order integer,
  possession_type integer,
  possession_id integer,
  count integer
);

CREATE VIRTUAL TABLE m_quest_scene USING masterdatabase (
  quest_scene_id integer,
  quest_id integer,
  sort_order integer,
  quest_scene_type integer,
  asset_background_id integer,
  event_map_number_upper integer,
  event_map_number_lower integer,
  is_main_flow_quest_target boolean,
  is_battle_only_target boolean,
  quest_result_type integer,
  is_story_skip_target boolean
);

CREATE VIRTUAL TABLE m_quest_scene_battle USING masterdatabase (
  quest_scene_id integer,
  battle_group_id integer,
  battle_drop_box_group_id integer,
  battle_field_locale_setting_index integer,
  battle_event_group_id integer,
  post_process_configuration_index integer
);

CREATE VIRTUAL TABLE m_quest_scene_choice USING masterdatabase (
  main_flow_quest_scene_id integer,
  quest_flow_type integer,
  choice_number integer,
  quest_scene_choice_effect_id integer
);

CREATE VIRTUAL TABLE m_quest_scene_choice_costume_effect_group USING masterdatabase (
  quest_scene_choice_costume_effect_group_id integer,
  sort_order integer,
  costume_id integer
);

CREATE VIRTUAL TABLE m_quest_scene_choice_effect USING masterdatabase (
  quest_scene_choice_effect_id integer,
  quest_scene_choice_grouping_id integer,
  quest_scene_choice_costume_effect_group_id integer,
  quest_scene_choice_weapon_effect_group_id integer
);

CREATE VIRTUAL TABLE m_quest_scene_choice_weapon_effect_group USING masterdatabase (
  quest_scene_choice_weapon_effect_group_id integer,
  sort_order integer,
  weapon_id integer
);

CREATE VIRTUAL TABLE m_quest_scene_not_confirm_title_dialog USING masterdatabase (
  quest_scene_id integer
);

CREATE VIRTUAL TABLE m_quest_scene_outgame_blendshape_motion USING masterdatabase (
  quest_scene_id integer,
  actor_animation_id integer
);

CREATE VIRTUAL TABLE m_quest_scene_picture_book_replace USING masterdatabase (
  quest_scene_id integer,
  picture_book_name_quest_text_id integer,
  is_exclude_subflow boolean,
  is_exclude_recollection boolean
);

CREATE VIRTUAL TABLE m_quest_schedule USING masterdatabase (
  quest_schedule_id integer,
  quest_schedule_cron_expression text,
  start_datetime timestamp,
  end_datetime timestamp
);

CREATE VIRTUAL TABLE m_quest_schedule_correspondence USING masterdatabase (
  quest_id integer,
  quest_schedule_id integer
);

CREATE VIRTUAL TABLE m_report USING masterdatabase (
  report_id integer,
  main_quest_season_id integer,
  character_id integer,
  report_number integer,
  report_asset_id integer
);

CREATE VIRTUAL TABLE m_shop USING masterdatabase (
  shop_id integer,
  shop_group_type integer,
  sort_order_in_shop_group integer,
  shop_type integer,
  name_shop_text_id integer,
  shop_updatable_label_type integer,
  shop_exchange_type integer,
  shop_item_cell_group_id integer,
  related_main_function_type integer,
  start_datetime timestamp,
  end_datetime timestamp,
  limited_open_id integer
);

CREATE VIRTUAL TABLE m_shop_display_price USING masterdatabase (
  price_type integer,
  price_id integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_shop_item USING masterdatabase (
  shop_item_id integer,
  name_shop_text_id integer,
  description_shop_text_id integer,
  shop_item_content_type integer,
  price_type integer,
  price_id integer,
  price integer,
  regular_price integer,
  shop_promotion_type integer,
  shop_item_limited_stock_id integer,
  asset_category_id integer,
  asset_variation_id integer,
  shop_item_decoration_type integer
);

CREATE VIRTUAL TABLE m_shop_item_additional_content USING masterdatabase (
  shop_item_additional_content_id integer,
  possession_type integer,
  possession_id integer,
  count integer
);

CREATE VIRTUAL TABLE m_shop_item_cell USING masterdatabase (
  shop_item_cell_id integer,
  step_number integer,
  shop_item_id integer
);

CREATE VIRTUAL TABLE m_shop_item_cell_group USING masterdatabase (
  shop_item_cell_group_id integer,
  shop_item_cell_id integer,
  sort_order integer,
  shop_item_cell_term_id integer
);

CREATE VIRTUAL TABLE m_shop_item_cell_limited_open USING masterdatabase (
  shop_item_cell_id integer,
  limited_open_id integer
);

CREATE VIRTUAL TABLE m_shop_item_cell_term USING masterdatabase (
  shop_item_cell_term_id integer,
  start_datetime timestamp,
  end_datetime timestamp
);

CREATE VIRTUAL TABLE m_shop_item_content_effect USING masterdatabase (
  shop_item_id integer,
  effect_target_type integer,
  effect_value_type integer,
  effect_value integer
);

CREATE VIRTUAL TABLE m_shop_item_content_mission USING masterdatabase (
  shop_item_id integer,
  mission_group_id integer,
  is_reevaluate_on_grant boolean
);

CREATE VIRTUAL TABLE m_shop_item_content_possession USING masterdatabase (
  shop_item_id integer,
  possession_type integer,
  possession_id integer,
  sort_order integer,
  count integer
);

CREATE VIRTUAL TABLE m_shop_item_limited_stock USING masterdatabase (
  shop_item_limited_stock_id integer,
  max_count integer,
  shop_item_auto_reset_type integer,
  shop_item_auto_reset_period integer
);

CREATE VIRTUAL TABLE m_shop_item_user_level_condition USING masterdatabase (
  shop_item_id integer,
  user_level_upper_limit integer,
  user_level_lower_limit integer,
  shop_item_additional_content_id integer
);

CREATE VIRTUAL TABLE m_shop_replaceable_gem USING masterdatabase (
  lineup_update_count_lower_limit integer,
  necessary_gem integer
);

CREATE VIRTUAL TABLE m_side_story_quest USING masterdatabase (
  side_story_quest_id integer,
  side_story_quest_type integer,
  target_id integer
);

CREATE VIRTUAL TABLE m_side_story_quest_limit_content USING masterdatabase (
  side_story_quest_limit_content_id integer,
  character_id integer,
  event_quest_chapter_id integer,
  difficulty_type integer,
  next_side_story_quest_id integer
);

CREATE VIRTUAL TABLE m_side_story_quest_scene USING masterdatabase (
  side_story_quest_id integer,
  side_story_quest_scene_id integer,
  sort_order integer,
  asset_background_id integer,
  event_map_number_upper integer,
  event_map_number_lower integer
);

CREATE VIRTUAL TABLE m_skill USING masterdatabase (
  skill_id integer,
  skill_level_group_id integer
);

CREATE VIRTUAL TABLE m_skill_abnormal USING masterdatabase (
  skill_abnormal_id integer,
  skill_abnormal_type_id integer,
  abnormal_polarity_type integer,
  skill_abnormal_lifetime_id integer,
  skill_abnormal_behaviour_group_id integer
);

CREATE VIRTUAL TABLE m_skill_abnormal_behaviour USING masterdatabase (
  skill_abnormal_behaviour_id integer,
  abnormal_behaviour_action_type integer,
  abnormal_behaviour_activation_method_type integer,
  abnormal_behaviour_deactivation_method_type integer,
  skill_abnormal_behaviour_action_id integer
);

CREATE VIRTUAL TABLE m_skill_abnormal_behaviour_action_abnormal_resistance USING masterdatabase (
  skill_abnormal_behaviour_action_id integer,
  abnormal_resistance_polarity_type integer,
  abnormal_resistance_skill_abnormal_type_id integer,
  block_probability_permil integer
);

CREATE VIRTUAL TABLE m_skill_abnormal_behaviour_action_attribute_damage_correction USING masterdatabase (
  skill_abnormal_behaviour_action_id integer,
  attribute_type integer,
  correction_target_damage_type integer,
  correction_value_permil integer,
  damage_correction_overlap_type integer,
  is_excepting boolean
);

CREATE VIRTUAL TABLE m_skill_abnormal_behaviour_action_buff_resistance USING masterdatabase (
  skill_abnormal_behaviour_action_id integer,
  buff_resistance_type integer,
  buff_resistance_status_kind_type integer,
  block_probability_permil integer
);

CREATE VIRTUAL TABLE m_skill_abnormal_behaviour_action_damage USING masterdatabase (
  skill_abnormal_behaviour_action_id integer,
  abnormal_behaviour_damage_type integer,
  power integer
);

CREATE VIRTUAL TABLE m_skill_abnormal_behaviour_action_damage_multiply USING masterdatabase (
  skill_abnormal_behaviour_action_id integer,
  damage_multiply_detail_type integer,
  damage_multiply_target_type integer,
  damage_multiply_abnormal_detail_id integer
);

CREATE VIRTUAL TABLE m_skill_abnormal_behaviour_action_damage_multiply_detail_always USING masterdatabase (
  damage_multiply_abnormal_detail_id integer,
  damage_multiply_coefficient_value_permil integer
);

CREATE VIRTUAL TABLE m_skill_abnormal_behaviour_action_default_skill_lottery USING masterdatabase (
  skill_abnormal_behaviour_action_id integer,
  target_count_lower integer,
  target_count_upper integer,
  value_permil integer,
  calculation_type integer
);

CREATE VIRTUAL TABLE m_skill_abnormal_behaviour_action_hit_ratio_down USING masterdatabase (
  skill_abnormal_behaviour_action_id integer,
  value integer
);

CREATE VIRTUAL TABLE m_skill_abnormal_behaviour_action_modify_hate_value USING masterdatabase (
  skill_abnormal_behaviour_action_id integer,
  hate_value_calculation_type integer,
  modify_value integer
);

CREATE VIRTUAL TABLE m_skill_abnormal_behaviour_action_override_evasion_value USING masterdatabase (
  skill_abnormal_behaviour_action_id integer,
  correction_value_permil integer
);

CREATE VIRTUAL TABLE m_skill_abnormal_behaviour_action_override_hit_effect USING masterdatabase (
  skill_abnormal_behaviour_action_id integer,
  override_effect_id integer,
  override_se_id integer,
  priority integer,
  disable_play_hit_voice boolean,
  play_on_miss boolean,
  force_rotate_on_hit boolean,
  override_hit_effect_condition_group_id integer,
  override_hit_effect_condition_operation_type integer
);

CREATE VIRTUAL TABLE m_skill_abnormal_behaviour_action_recovery USING masterdatabase (
  skill_abnormal_behaviour_action_id integer,
  abnormal_behaviour_recovery_type integer,
  value integer,
  upper integer
);

CREATE VIRTUAL TABLE m_skill_abnormal_behaviour_action_turn_restriction USING masterdatabase (
  skill_behaviour_action_id integer,
  turn_restriction_probability_permil integer,
  abnormal_behaviour_turn_restriction_skill_type integer
);

CREATE VIRTUAL TABLE m_skill_abnormal_behaviour_group USING masterdatabase (
  skill_abnormal_behaviour_group_id integer,
  abnormal_behaviour_index integer,
  skill_abnormal_behaviour_id integer
);

CREATE VIRTUAL TABLE m_skill_abnormal_damage_multiply_detail_abnormal USING masterdatabase (
  damage_multiply_abnormal_detail_id integer,
  skill_damage_multiply_abnormal_attached_value_group_id integer
);

CREATE VIRTUAL TABLE m_skill_abnormal_damage_multiply_detail_buff_attached USING masterdatabase (
  damage_multiply_abnormal_detail_id integer,
  buff_attached_target_type integer,
  target_buff_type integer,
  target_status_kind_type integer,
  damage_multiply_coefficient_value_permil integer
);

CREATE VIRTUAL TABLE m_skill_abnormal_damage_multiply_detail_critical USING masterdatabase (
  damage_multiply_abnormal_detail_id integer,
  is_critical boolean,
  damage_multiply_coefficient_value_permil integer
);

CREATE VIRTUAL TABLE m_skill_abnormal_damage_multiply_detail_hit_index USING masterdatabase (
  damage_multiply_abnormal_detail_id integer,
  skill_damage_multiply_hit_index_value_group_id integer
);

CREATE VIRTUAL TABLE m_skill_abnormal_damage_multiply_detail_skillful_weapon USING masterdatabase (
  damage_multiply_abnormal_detail_id integer,
  condition_target_type integer,
  weapon_type integer,
  is_skillful_main_weapon boolean,
  damage_multiply_coefficient_value_permil integer
);

CREATE VIRTUAL TABLE m_skill_abnormal_lifetime USING masterdatabase (
  skill_abnormal_lifetime_id integer,
  skill_abnormal_lifetime_behaviour_group_id integer,
  abnormal_lifetime_behaviour_condition_type integer
);

CREATE VIRTUAL TABLE m_skill_abnormal_lifetime_behaviour_activate_count USING masterdatabase (
  skill_abnormal_lifetime_behaviour_id integer,
  activate_count integer,
  abnormal_behaviour_index integer
);

CREATE VIRTUAL TABLE m_skill_abnormal_lifetime_behaviour_frame_count USING masterdatabase (
  skill_abnormal_lifetime_behaviour_id integer,
  frame_count integer
);

CREATE VIRTUAL TABLE m_skill_abnormal_lifetime_behaviour_group USING masterdatabase (
  skill_abnormal_lifetime_behaviour_group_id integer,
  abnormal_lifetime_behaviour_index integer,
  abnormal_lifetime_method_type integer,
  skill_abnormal_lifetime_behaviour_id integer
);

CREATE VIRTUAL TABLE m_skill_abnormal_lifetime_behaviour_receive_damage_count USING masterdatabase (
  skill_abnormal_lifetime_behaviour_id integer,
  receive_damage_count integer
);

CREATE VIRTUAL TABLE m_skill_abnormal_lifetime_behaviour_turn_count USING masterdatabase (
  skill_abnormal_lifetime_behaviour_id integer,
  turn_count integer
);

CREATE VIRTUAL TABLE m_skill_behaviour USING masterdatabase (
  skill_behaviour_id integer,
  skill_behaviour_type integer,
  skill_behaviour_action_id integer,
  skill_behaviour_activation_method_id integer,
  skill_behaviour_asset_calculator_id integer,
  hit_ratio_permil integer,
  skill_behaviour_lifetime_calculation_method_type integer,
  lifetime_count integer,
  skill_target_scope_asset_calculator_id integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_action_abnormal USING masterdatabase (
  skill_behaviour_action_id integer,
  skill_behaviour_abnormal_id integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_action_active_skill_damage_correction USING masterdatabase (
  skill_behaviour_action_id integer,
  correction_value_permil integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_action_advance_active_skill_cooltime USING masterdatabase (
  skill_behaviour_action_id integer,
  skill_cooltime_advance_type integer,
  active_skill_type integer,
  advance_value integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_action_advance_active_skill_cooltime_immediate USING masterdatabase (
  skill_behaviour_action_id integer,
  skill_cooltime_advance_type integer,
  active_skill_type integer,
  advance_value integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_action_attack USING masterdatabase (
  skill_behaviour_action_id integer,
  skill_power integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_action_attack_clamp_hp USING masterdatabase (
  skill_behaviour_action_id integer,
  skill_power integer,
  clamp_threshold_hp_ratio_permil integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_action_attack_combo USING masterdatabase (
  skill_behaviour_action_id integer,
  skill_power_calculation_id integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_action_attack_fixed_damage USING masterdatabase (
  skill_behaviour_action_id integer,
  damage_value integer,
  force_damage boolean
);

CREATE VIRTUAL TABLE m_skill_behaviour_action_attack_hp_ratio USING masterdatabase (
  skill_behaviour_action_id integer,
  skill_power_calculation_id integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_action_attack_ignore_vitality USING masterdatabase (
  skill_behaviour_action_id integer,
  skill_power integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_action_attack_main_weapon_attribute USING masterdatabase (
  skill_behaviour_action_id integer,
  skill_power integer,
  attribute_type integer,
  magnification_rate integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_action_attack_skillful_main_weapon_type USING masterdatabase (
  skill_behaviour_action_id integer,
  skill_power integer,
  magnification_rate integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_action_attack_vitality USING masterdatabase (
  skill_behaviour_action_id integer,
  skill_power integer,
  attack_weight integer,
  vitality_weight integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_action_attribute_damage_correction USING masterdatabase (
  skill_behaviour_action_id integer,
  attribute_type integer,
  correction_target_damage_type integer,
  correction_value_permil integer,
  damage_correction_overlap_type integer,
  is_excepting boolean
);

CREATE VIRTUAL TABLE m_skill_behaviour_action_buff USING masterdatabase (
  skill_behaviour_action_id integer,
  skill_buff_id integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_action_changestep USING masterdatabase (
  skill_behaviour_action_id integer,
  step integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_action_damage_correction_hp_ratio USING masterdatabase (
  skill_behaviour_action_id integer,
  correction_max_value_permil integer,
  damage_correction_hp_ratio_type integer,
  activation_threshold_hp_ratio_permil integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_action_damage_multiply USING masterdatabase (
  skill_behaviour_action_id integer,
  damage_multiply_detail_type integer,
  skill_damage_multiply_detail_id integer,
  damage_multiply_target_type integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_action_default_skill_lottery USING masterdatabase (
  skill_behaviour_action_id integer,
  target_count_lower integer,
  target_count_upper integer,
  value_permil integer,
  calculation_type integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_action_extend_buff_cooltime USING masterdatabase (
  skill_behaviour_action_id integer,
  extend_buff_cooltime_buff_type integer,
  extend_buff_cooltime_status_type integer,
  extend_buff_cooltime_target_skill_type integer,
  extend_buff_cooltime_extend_type integer,
  extend_value integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_action_hp_ratio_damage USING masterdatabase (
  skill_behaviour_action_id integer,
  calculate_denominator_type integer,
  damage_ratio_permil integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_action_overlimit_damage_multiply USING masterdatabase (
  skill_behaviour_action_id integer,
  damage_multiply_detail_type integer,
  skill_damage_multiply_detail_id integer,
  damage_multiply_target_type integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_action_recovery USING masterdatabase (
  skill_behaviour_action_id integer,
  skill_power integer,
  fixed_recovery_point integer,
  hp_ratio_recovery_point_permil integer,
  recovery_point_min_value integer,
  recovery_point_max_value integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_action_recovery_point_correction USING masterdatabase (
  skill_behaviour_action_id integer,
  recovery_point_correction_target_skill_type integer,
  recovery_point_correction_coefficient_value integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_action_remove_abnormal USING masterdatabase (
  skill_behaviour_action_id integer,
  target_polarity_type integer,
  skill_remove_abnormal_target_abnormal_group_id integer,
  remove_abnormal_target_type integer,
  remove_count_upper integer,
  remove_kind_count_upper integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_action_remove_buff USING masterdatabase (
  skill_behaviour_action_id integer,
  remove_count integer,
  buff_type integer,
  skill_remove_buff_filtering_type integer,
  skill_remove_buff_filtering_id integer,
  skill_remove_buff_choosing_type integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_action_shorten_active_skill_cooltime USING masterdatabase (
  skill_behaviour_action_id integer,
  active_skill_type integer,
  shorten_value_permil integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_action_skill_recovery_power_correction USING masterdatabase (
  skill_behaviour_action_id integer,
  correction_value_permil integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_activation_condition_activation_upper_count USING masterdatabase (
  skill_behaviour_activation_condition_id integer,
  activation_upper_count integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_activation_condition_attribute USING masterdatabase (
  skill_behaviour_activation_condition_id integer,
  skill_behaviour_activation_condition_attribute_type integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_activation_condition_group USING masterdatabase (
  skill_behaviour_activation_condition_group_id integer,
  condition_check_order integer,
  skill_behaviour_activation_condition_type integer,
  skill_behaviour_activation_condition_id integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_activation_condition_hp_ratio USING masterdatabase (
  skill_behaviour_activation_condition_id integer,
  skill_behaviour_activation_condition_hp_ratio_threshold_type integer,
  threshold_ratio_permil integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_activation_condition_in_skill_flow USING masterdatabase (
  skill_behaviour_activation_condition_id integer,
  running_skill_behaviour_type integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_activation_condition_wave_number USING masterdatabase (
  skill_behaviour_activation_condition_id integer,
  wave_number integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_activation_method USING masterdatabase (
  skill_behaviour_activation_method_id integer,
  activation_method_type integer,
  skill_behaviour_activation_condition_group_id integer
);

CREATE VIRTUAL TABLE m_skill_behaviour_group USING masterdatabase (
  skill_behaviour_group_id integer,
  skill_behaviour_id integer,
  skill_behaviour_index integer,
  target_selector_index integer,
  skill_hit_start_index integer,
  skill_hit_end_index integer
);

CREATE VIRTUAL TABLE m_skill_buff USING masterdatabase (
  skill_buff_id integer,
  buff_key text,
  buff_type integer,
  power integer,
  cooltime_frame_count integer,
  icon_id integer
);

CREATE VIRTUAL TABLE m_skill_casttime USING masterdatabase (
  skill_casttime_id integer,
  skill_casttime_value integer,
  skill_casttime_behaviour_group_id integer
);

CREATE VIRTUAL TABLE m_skill_casttime_behaviour USING masterdatabase (
  skill_casttime_behaviour_id integer,
  skill_casttime_behaviour_type integer,
  skill_casttime_behaviour_action_id integer
);

CREATE VIRTUAL TABLE m_skill_casttime_behaviour_action_on_frame_update USING masterdatabase (
  skill_casttime_behaviour_action_id integer,
  skill_casttime_advance_value_per_frame integer
);

CREATE VIRTUAL TABLE m_skill_casttime_behaviour_action_on_skill_damage_condition USING masterdatabase (
  skill_casttime_behaviour_action_id integer,
  skill_casttime_update_value integer,
  skill_casttime_advance_type integer,
  damage_compare_type integer,
  damage_condition_value integer
);

CREATE VIRTUAL TABLE m_skill_casttime_behaviour_group USING masterdatabase (
  skill_casttime_behaviour_group_id integer,
  skill_casttime_behaviour_index integer,
  skill_casttime_behaviour_id integer
);

CREATE VIRTUAL TABLE m_skill_cooltime_advance_value_on_default_skill_group USING masterdatabase (
  skill_cooltime_advance_value_on_default_skill_group_id integer,
  skill_hit_count_lower_limit integer,
  skill_cooltime_advance_value integer
);

CREATE VIRTUAL TABLE m_skill_cooltime_behaviour USING masterdatabase (
  skill_cooltime_behaviour_id integer,
  skill_cooltime_behaviour_type integer,
  skill_cooltime_behaviour_action_id integer
);

CREATE VIRTUAL TABLE m_skill_cooltime_behaviour_group USING masterdatabase (
  skill_cooltime_behaviour_group_id integer,
  skill_cooltime_behaviour_id integer
);

CREATE VIRTUAL TABLE m_skill_cooltime_behaviour_on_execute_active_skill USING masterdatabase (
  skill_cooltime_behaviour_action_id integer,
  skill_cooltime_advance_value integer
);

CREATE VIRTUAL TABLE m_skill_cooltime_behaviour_on_execute_companion_skill USING masterdatabase (
  skill_cooltime_behaviour_action_id integer,
  skill_cooltime_advance_value integer
);

CREATE VIRTUAL TABLE m_skill_cooltime_behaviour_on_execute_default_skill USING masterdatabase (
  skill_cooltime_behaviour_action_id integer,
  skill_cooltime_advance_value_on_default_skill_group_id integer
);

CREATE VIRTUAL TABLE m_skill_cooltime_behaviour_on_frame_update USING masterdatabase (
  skill_cooltime_behaviour_action_id integer,
  skill_cooltime_advance_value_per_frame integer
);

CREATE VIRTUAL TABLE m_skill_cooltime_behaviour_on_skill_damage USING masterdatabase (
  skill_cooltime_behaviour_action_id integer,
  skill_cooltime_advance_value_calculation_id integer
);

CREATE VIRTUAL TABLE m_skill_damage_multiply_abnormal_attached_value_group USING masterdatabase (
  skill_damage_multiply_abnormal_attached_value_group_id integer,
  skill_damage_multiply_abnormal_attached_value_group_index integer,
  polarity_condition_type integer,
  skill_abnormal_type_id_condition integer,
  target_type integer,
  damage_multiply_coefficient_value_permil integer
);

CREATE VIRTUAL TABLE m_skill_damage_multiply_detail_abnormal_attached USING masterdatabase (
  skill_damage_multiply_detail_id integer,
  skill_damage_multiply_abnormal_attached_value_group_id integer
);

CREATE VIRTUAL TABLE m_skill_damage_multiply_detail_always USING masterdatabase (
  skill_damage_multiply_detail_id integer,
  damage_multiply_coefficient_value_permil integer
);

CREATE VIRTUAL TABLE m_skill_damage_multiply_detail_buff_attached USING masterdatabase (
  skill_damage_multiply_detail_id integer,
  buff_attached_target_type integer,
  target_buff_type integer,
  target_status_kind_type integer,
  damage_multiply_coefficient_value_permil integer
);

CREATE VIRTUAL TABLE m_skill_damage_multiply_detail_critical USING masterdatabase (
  skill_damage_multiply_detail_id integer,
  is_critical boolean,
  damage_multiply_coefficient_value_permil integer
);

CREATE VIRTUAL TABLE m_skill_damage_multiply_detail_hit_index USING masterdatabase (
  skill_damage_multiply_detail_id integer,
  skill_damage_multiply_hit_index_value_group_id integer
);

CREATE VIRTUAL TABLE m_skill_damage_multiply_detail_skillful_weapon_type USING masterdatabase (
  skill_damage_multiply_detail_id integer,
  condition_target_type integer,
  weapon_type integer,
  is_skillful_main_weapon boolean,
  damage_multiply_coefficient_value_permil integer
);

CREATE VIRTUAL TABLE m_skill_damage_multiply_detail_specified_costume_type USING masterdatabase (
  skill_damage_multiply_detail_id integer,
  specified_costume_condition_target_type integer,
  target_specified_costume_group_id integer,
  damage_multiply_coefficient_value_permil integer
);

CREATE VIRTUAL TABLE m_skill_damage_multiply_hit_index_value_group USING masterdatabase (
  skill_damage_multiply_hit_index_value_group_id integer,
  skill_damage_multiply_hit_index_value_group_index integer,
  total_hit_count_condition_lower integer,
  total_hit_count_condition_upper integer,
  hit_index integer,
  damage_multiply_coefficient_value_permil integer
);

CREATE VIRTUAL TABLE m_skill_damage_multiply_target_specified_costume_group USING masterdatabase (
  target_specified_costume_group_id integer,
  target_specified_costume_group_index integer,
  costume_id integer
);

CREATE VIRTUAL TABLE m_skill_detail USING masterdatabase (
  skill_detail_id integer,
  skill_behaviour_group_id integer,
  name_skill_text_id integer,
  skill_cooltime_value integer,
  skill_cooltime_behaviour_group_id integer,
  casttime_frame_count integer,
  hit_ratio_permil integer,
  skill_range_milli integer,
  skill_hit_asset_calculator_id integer,
  is_counter_applicable boolean,
  is_combo_calculation_target boolean,
  skill_asset_category_id integer,
  skill_asset_variation_id integer,
  description_skill_text_id integer,
  skill_act_type integer,
  skill_hit_count integer,
  skill_power_base_value integer,
  power_calculation_reference_status_type integer,
  power_reference_status_group_id integer,
  skill_casttime_id integer
);

CREATE VIRTUAL TABLE m_skill_level_group USING masterdatabase (
  skill_level_group_id integer,
  level_lower_limit integer,
  skill_detail_id integer
);

CREATE VIRTUAL TABLE m_skill_remove_abnormal_target_abnormal_group USING masterdatabase (
  skill_remove_abnormal_target_abnormal_group_id integer,
  skill_remove_abnormal_target_abnormal_group_index integer,
  abnormal_type_id integer
);

CREATE VIRTUAL TABLE m_skill_remove_buff_filter_status_kind USING masterdatabase (
  skill_remove_buff_filtering_id integer,
  filter_index integer,
  status_kind_type integer
);

CREATE VIRTUAL TABLE m_skill_reserve_ui_type USING masterdatabase (
  skill_detail_id integer,
  skill_reserve_ui_type_id integer
);

CREATE VIRTUAL TABLE m_smartphone_chat_group USING masterdatabase (
  smartphone_chat_group_id integer,
  smartphone_group_unlock_scene_id integer,
  smartphone_group_unlock_value integer,
  sort_order integer,
  smartphone_group_end_scene_id integer
);

CREATE VIRTUAL TABLE m_smartphone_chat_group_message USING masterdatabase (
  smartphone_chat_group_id integer,
  sort_order integer,
  smartphone_message_unlock_scene_id integer,
  smartphone_message_unlock_value integer,
  sender_character_id integer
);

CREATE VIRTUAL TABLE m_speaker USING masterdatabase (
  actor_object_id integer,
  name_speaker_text_id integer,
  speaker_icon_type integer,
  speaker_icon_index integer
);

CREATE VIRTUAL TABLE m_stained_glass USING masterdatabase (
  stained_glass_id integer,
  stained_glass_category_type integer,
  image_asset_id integer,
  title_text_id integer,
  effect_description_text_id integer,
  flavor_text_id integer,
  sort_order integer,
  empty_display_start_datetime timestamp,
  lock_display_start_datetime timestamp,
  stained_glass_status_up_target_group_id integer,
  stained_glass_status_up_group_id integer
);

CREATE VIRTUAL TABLE m_stained_glass_status_up_group USING masterdatabase (
  stained_glass_status_up_group_id integer,
  group_index integer,
  status_kind_type integer,
  status_calculation_type integer,
  effect_value integer
);

CREATE VIRTUAL TABLE m_stained_glass_status_up_target_group USING masterdatabase (
  stained_glass_status_up_target_group_id integer,
  group_index integer,
  status_up_target_type integer,
  target_value integer
);

CREATE VIRTUAL TABLE m_thought USING masterdatabase (
  thought_id integer,
  rarity_type integer,
  ability_id integer,
  ability_level integer,
  thought_asset_id integer
);

CREATE VIRTUAL TABLE m_tip USING masterdatabase (
  tip_id integer,
  title_tip_text_id integer,
  content_tip_text_id integer,
  tip_display_condition_group_id integer,
  background_asset_name text,
  start_datetime timestamp,
  end_datetime timestamp
);

CREATE VIRTUAL TABLE m_tip_background_asset USING masterdatabase (
  tip_background_asset_id integer,
  background_asset_name text
);

CREATE VIRTUAL TABLE m_tip_display_condition_group USING masterdatabase (
  tip_display_condition_group_id integer,
  sort_order integer,
  tip_display_condition_type integer,
  condition_value integer,
  condition_operation_type integer
);

CREATE VIRTUAL TABLE m_tip_group USING masterdatabase (
  tip_group_id integer,
  name_text_id integer
);

CREATE VIRTUAL TABLE m_tip_group_background_asset USING masterdatabase (
  tip_group_id integer,
  background_asset_name text,
  display_condition_clear_quest_id integer
);

CREATE VIRTUAL TABLE m_tip_group_background_asset_relation USING masterdatabase (
  tip_group_id integer,
  tip_background_asset_id integer,
  tip_display_condition_group_id integer
);

CREATE VIRTUAL TABLE m_tip_group_selection USING masterdatabase (
  tip_group_id integer,
  tip_id integer,
  encounter_rate_permil integer
);

CREATE VIRTUAL TABLE m_tip_group_situation USING masterdatabase (
  tip_situation_type integer,
  tip_group_id integer,
  weight integer,
  target_id integer
);

CREATE VIRTUAL TABLE m_tip_group_situation_season USING masterdatabase (
  tip_situation_type integer,
  tip_situation_season_id integer,
  tip_group_id integer,
  weight integer,
  target_id integer
);

CREATE VIRTUAL TABLE m_title_flow_movie USING masterdatabase (
  title_flow_movie_id integer,
  movie_id integer,
  start_datetime timestamp,
  end_datetime timestamp
);

CREATE VIRTUAL TABLE m_title_still USING masterdatabase (
  title_still_id integer,
  title_still_group_id integer,
  release_evaluate_condition_id integer,
  title_still_logo_type integer,
  asset_name text
);

CREATE VIRTUAL TABLE m_title_still_group USING masterdatabase (
  title_still_group_id integer,
  display_still_count integer,
  weight integer,
  priority integer
);

CREATE VIRTUAL TABLE m_tutorial_consume_possession_group USING masterdatabase (
  tutorial_type integer,
  possession_type integer,
  possession_id integer,
  count integer
);

CREATE VIRTUAL TABLE m_tutorial_dialog USING masterdatabase (
  tutorial_type integer,
  help_type integer
);

CREATE VIRTUAL TABLE m_tutorial_unlock_condition USING masterdatabase (
  tutorial_type integer,
  tutorial_unlock_condition_type integer,
  condition_value integer
);

CREATE VIRTUAL TABLE m_user_level USING masterdatabase (
  user_level integer,
  max_stamina integer
);

CREATE VIRTUAL TABLE m_user_quest_scene_grant_possession USING masterdatabase (
  quest_scene_id integer,
  possession_type integer,
  possession_id integer,
  count integer,
  is_gift boolean,
  is_debug boolean
);

CREATE VIRTUAL TABLE m_weapon USING masterdatabase (
  weapon_id integer,
  weapon_category_type integer,
  weapon_type integer,
  asset_variation_id integer,
  rarity_type integer,
  attribute_type integer,
  is_restrict_discard boolean,
  weapon_base_status_id integer,
  weapon_status_calculation_id integer,
  weapon_skill_group_id integer,
  weapon_ability_group_id integer,
  weapon_evolution_material_group_id integer,
  weapon_evolution_grant_possession_group_id integer,
  weapon_story_release_condition_group_id integer,
  weapon_specific_enhance_id integer,
  weapon_specific_limit_break_material_group_id integer,
  character_walkaround_range_type integer,
  is_recyclable boolean
);

CREATE VIRTUAL TABLE m_weapon_ability_enhancement_material USING masterdatabase (
  weapon_ability_enhancement_material_id integer,
  ability_level integer,
  material_id integer,
  count integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_weapon_ability_group USING masterdatabase (
  weapon_ability_group_id integer,
  slot_number integer,
  ability_id integer,
  weapon_ability_enhancement_material_id integer
);

CREATE VIRTUAL TABLE m_weapon_awaken USING masterdatabase (
  weapon_id integer,
  weapon_awaken_effect_group_id integer,
  weapon_awaken_material_group_id integer,
  consume_gold integer,
  level_limit_up integer
);

CREATE VIRTUAL TABLE m_weapon_awaken_ability USING masterdatabase (
  weapon_awaken_ability_id integer,
  ability_id integer,
  ability_level integer
);

CREATE VIRTUAL TABLE m_weapon_awaken_effect_group USING masterdatabase (
  weapon_awaken_effect_group_id integer,
  weapon_awaken_effect_type integer,
  weapon_awaken_effect_id integer
);

CREATE VIRTUAL TABLE m_weapon_awaken_material_group USING masterdatabase (
  weapon_awaken_material_group_id integer,
  material_id integer,
  count integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_weapon_awaken_status_up_group USING masterdatabase (
  weapon_awaken_status_up_group_id integer,
  status_kind_type integer,
  status_calculation_type integer,
  effect_value integer
);

CREATE VIRTUAL TABLE m_weapon_base_status USING masterdatabase (
  weapon_base_status_id integer,
  hp integer,
  attack integer,
  vitality integer
);

CREATE VIRTUAL TABLE m_weapon_consume_exchange_consumable_item_group USING masterdatabase (
  weapon_id integer,
  consumable_item_id integer,
  count integer
);

CREATE VIRTUAL TABLE m_weapon_enhanced USING masterdatabase (
  weapon_enhanced_id integer,
  weapon_id integer,
  level integer,
  limit_break_count integer
);

CREATE VIRTUAL TABLE m_weapon_enhanced_ability USING masterdatabase (
  weapon_enhanced_id integer,
  ability_id integer,
  level integer
);

CREATE VIRTUAL TABLE m_weapon_enhanced_skill USING masterdatabase (
  weapon_enhanced_id integer,
  skill_id integer,
  level integer
);

CREATE VIRTUAL TABLE m_weapon_evolution_group USING masterdatabase (
  weapon_evolution_group_id integer,
  evolution_order integer,
  weapon_id integer
);

CREATE VIRTUAL TABLE m_weapon_evolution_material_group USING masterdatabase (
  weapon_evolution_material_group_id integer,
  material_id integer,
  count integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_weapon_field_effect_decrease_point USING masterdatabase (
  weapon_id integer,
  relation_index integer,
  field_effect_group_id integer,
  field_effect_ability_id integer,
  decrease_point integer,
  weapon_ability_id integer,
  is_weapon_awaken boolean,
  auto_organization_coefficient_permil integer
);

CREATE VIRTUAL TABLE m_weapon_rarity USING masterdatabase (
  rarity_type integer,
  base_enhancement_obtained_exp integer,
  sell_price_numerical_function_id integer,
  max_level_numerical_function_id integer,
  max_skill_level_numerical_function_id integer,
  max_ability_level_numerical_function_id integer,
  required_exp_for_level_up_numerical_parameter_map_id integer,
  enhancement_cost_by_weapon_numerical_function_id integer,
  enhancement_cost_by_material_numerical_function_id integer,
  skill_enhancement_cost_numerical_function_id integer,
  ability_enhancement_cost_numerical_function_id integer,
  limit_break_cost_by_weapon_numerical_function_id integer,
  limit_break_cost_by_material_numerical_function_id integer,
  evolution_cost_numerical_function_id integer
);

CREATE VIRTUAL TABLE m_weapon_rarity_limit_break_material_group USING masterdatabase (
  rarity_type integer,
  material_id integer,
  count integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_weapon_skill_enhancement_material USING masterdatabase (
  weapon_skill_enhancement_material_id integer,
  skill_level integer,
  material_id integer,
  count integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_weapon_skill_group USING masterdatabase (
  weapon_skill_group_id integer,
  slot_number integer,
  skill_id integer,
  weapon_skill_enhancement_material_id integer
);

CREATE VIRTUAL TABLE m_weapon_specific_enhance USING masterdatabase (
  weapon_specific_enhance_id integer,
  base_enhancement_obtained_exp integer,
  sell_price_numerical_function_id integer,
  max_level_numerical_function_id integer,
  max_skill_level_numerical_function_id integer,
  max_ability_level_numerical_function_id integer,
  required_exp_for_level_up_numerical_parameter_map_id integer,
  enhancement_cost_by_weapon_numerical_function_id integer,
  enhancement_cost_by_material_numerical_function_id integer,
  skill_enhancement_cost_numerical_function_id integer,
  ability_enhancement_cost_numerical_function_id integer,
  limit_break_cost_by_weapon_numerical_function_id integer,
  limit_break_cost_by_material_numerical_function_id integer,
  evolution_cost_numerical_function_id integer
);

CREATE VIRTUAL TABLE m_weapon_specific_limit_break_material_group USING masterdatabase (
  weapon_specific_limit_break_material_group_id integer,
  limit_break_count_lower_limit integer,
  material_id integer,
  count integer,
  sort_order integer
);

CREATE VIRTUAL TABLE m_weapon_status_calculation USING masterdatabase (
  weapon_status_calculation_id integer,
  hp_numerical_function_id integer,
  attack_numerical_function_id integer,
  vitality_numerical_function_id integer
);

CREATE VIRTUAL TABLE m_weapon_story_release_condition_group USING masterdatabase (
  weapon_story_release_condition_group_id integer,
  story_index integer,
  weapon_story_release_condition_type integer,
  condition_value integer,
  weapon_story_release_condition_operation_group_id integer
);

CREATE VIRTUAL TABLE m_weapon_story_release_condition_operation USING masterdatabase (
  weapon_story_release_condition_operation_group_id integer,
  sort_order integer,
  weapon_story_release_condition_type integer,
  condition_value integer
);

CREATE VIRTUAL TABLE m_weapon_story_release_condition_operation_group USING masterdatabase (
  weapon_story_release_condition_operation_group_id integer,
  condition_operation_type integer
);

CREATE VIRTUAL TABLE m_webview_mission USING masterdatabase (
  webview_mission_id integer,
  title_text_id integer,
  webview_mission_type integer,
  webview_mission_target_id integer,
  start_datetime timestamp,
  end_datetime timestamp
);

CREATE VIRTUAL TABLE m_webview_mission_title_text USING masterdatabase (
  webview_mission_title_text_id integer,
  language_type integer,
  text text
);

CREATE VIRTUAL TABLE m_webview_panel_mission USING masterdatabase (
  webview_panel_mission_id integer,
  page integer,
  webview_panel_mission_page_id integer,
  start_datetime timestamp,
  end_datetime timestamp
);

CREATE VIRTUAL TABLE m_webview_panel_mission_complete_flavor_text USING masterdatabase (
  webview_panel_mission_complete_flavor_text_id integer,
  language_type integer,
  complete_flavor_text text
);

CREATE VIRTUAL TABLE m_webview_panel_mission_page USING masterdatabase (
  webview_panel_mission_page_id integer,
  webview_panel_mission_panel_group_id integer,
  webview_panel_mission_bg_parts_group_id integer,
  webview_panel_mission_complete_flavor_text_id integer,
  image_file_name text,
  image_asset_id integer,
  webview_panel_mission_page_reward_group_id integer
);

CREATE TABLE i_user (
  user_id INTEGER PRIMARY KEY NOT NULL,
  player_id bigint NOT NULL DEFAULT 1,
  os_type integer NOT NULL DEFAULT 2,
  platform_type integer NOT NULL DEFAULT 2,
  user_restriction_type integer NOT NULL DEFAULT 1,
  register_datetime timestamp NOT NULL,
  game_start_datetime timestamp NOT NULL DEFAULT 28800000,
  latest_version bigint NOT NULL DEFAULT 1
);

CREATE TRIGGER i_user_update_version AFTER UPDATE ON i_user FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user SET latest_version = OLD.latest_version + 1 WHERE i_user.rowid = NEW.rowid;
END;

CREATE TABLE i_user_apple (
  user_id INTEGER PRIMARY KEY NOT NULL,
  apple_id text,
  latest_version bigint NOT NULL  DEFAULT 1
);

CREATE TRIGGER i_user_apple_update_version AFTER UPDATE ON i_user_apple FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_apple SET latest_version = OLD.latest_version + 1 WHERE i_user_apple.rowid = NEW.rowid;
END;

CREATE TABLE i_user_auto_sale_setting_detail (
  user_id INTEGER NOT NULL,
  possession_auto_sale_item_type integer NOT NULL,
  possession_auto_sale_item_value text,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, possession_auto_sale_item_type)
);
CREATE INDEX i_user_auto_sale_setting_detail_user_id ON i_user_auto_sale_setting_detail (user_id);

CREATE TRIGGER i_user_auto_sale_setting_detail_update_version
AFTER UPDATE ON i_user_auto_sale_setting_detail FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_auto_sale_setting_detail SET latest_version = OLD.latest_version + 1 WHERE i_user_auto_sale_setting_detail.rowid = NEW.rowid;
END;

CREATE TABLE i_user_beginner_campaign (
  user_id INTEGER NOT NULL,
  beginner_campaign_id integer NOT NULL,
  campaign_register_datetime timestamp,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, beginner_campaign_id)
);
CREATE INDEX i_user_beginner_campaign_user_id ON i_user_beginner_campaign(user_id);

CREATE TRIGGER i_user_beginner_campaign_update_version AFTER UPDATE ON i_user_beginner_campaign FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_beginner_campaign SET latest_version = OLD.latest_version + 1 WHERE i_user_beginner_campaign.rowid = NEW.rowid;
END;

CREATE TABLE i_user_big_hunt_max_score (
  user_id INTEGER NOT NULL,
  big_hunt_boss_id integer NOT NULL,
  max_score bigint,
  max_score_update_datetime timestamp,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, big_hunt_boss_id)
);
CREATE INDEX i_user_big_hunt_max_score_user_id ON i_user_big_hunt_max_score(user_id);

CREATE TRIGGER i_user_big_hunt_max_score_update_version AFTER UPDATE ON i_user_big_hunt_max_score FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_big_hunt_max_score SET latest_version = OLD.latest_version + 1 WHERE i_user_big_hunt_max_score.rowid = NEW.rowid;
END;

CREATE TABLE i_user_big_hunt_progress_status (
  user_id INTEGER NOT NULL PRIMARY KEY,
  current_big_hunt_boss_quest_id integer,
  current_big_hunt_quest_id integer,
  current_quest_scene_id integer,
  is_dry_run boolean,
  latest_version bigint NOT NULL DEFAULT 1
);

CREATE TRIGGER i_user_big_hunt_progress_status_update_version
AFTER UPDATE ON i_user_big_hunt_progress_status FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_big_hunt_progress_status SET latest_version = OLD.latest_version + 1 WHERE i_user_big_hunt_progress_status.rowid = NEW.rowid;
END;

CREATE TABLE i_user_big_hunt_schedule_max_score (
  user_id INTEGER NOT NULL,
  big_hunt_schedule_id integer NOT NULL,
  big_hunt_boss_id integer,
  max_score bigint,
  max_score_update_datetime timestamp,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, big_hunt_boss_id)
);
CREATE INDEX i_user_big_hunt_schedule_max_score_user_id ON i_user_big_hunt_schedule_max_score(user_id);

CREATE TRIGGER i_user_big_hunt_schedule_max_score_update_version
AFTER UPDATE ON i_user_big_hunt_schedule_max_score FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_big_hunt_schedule_max_score SET latest_version = OLD.latest_version + 1 WHERE i_user_big_hunt_schedule_max_score.rowid = NEW.rowid;
END;

CREATE TABLE i_user_big_hunt_status (
  user_id INTEGER NOT NULL PRIMARY KEY,
  big_hunt_boss_quest_id integer,
  daily_challenge_count integer,
  latest_challenge_datetime timestamp,
  latest_version bigint NOT NULL DEFAULT 1
);

CREATE TRIGGER i_user_big_hunt_status_update_version
AFTER UPDATE ON i_user_big_hunt_status FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_big_hunt_status SET latest_version = OLD.latest_version + 1 WHERE i_user_big_hunt_status.rowid = NEW.rowid;
END;

CREATE TABLE i_user_big_hunt_weekly_max_score (
  user_id INTEGER NOT NULL,
  big_hunt_weekly_version bigint NOT NULL,
  attribute_type integer,
  max_score bigint,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, big_hunt_weekly_version)
);
CREATE INDEX i_user_big_hunt_weekly_max_score_user_id ON i_user_big_hunt_weekly_max_score (user_id);

CREATE TRIGGER i_user_big_hunt_weekly_max_score_update_version
AFTER UPDATE ON i_user_big_hunt_weekly_max_score FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_big_hunt_weekly_max_score SET latest_version = OLD.latest_version + 1 WHERE i_user_big_hunt_weekly_max_score.rowid = NEW.rowid;
END;

CREATE TABLE i_user_big_hunt_weekly_status (
  user_id INTEGER NOT NULL,
  big_hunt_weekly_version bigint NOT NULL,
  is_received_weekly_reward boolean,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, big_hunt_weekly_version)
);
CREATE INDEX i_user_big_hunt_weekly_status_user_id ON i_user_big_hunt_weekly_status (user_id);

CREATE TRIGGER i_user_big_hunt_weekly_status_update_version
AFTER UPDATE ON i_user_big_hunt_weekly_status FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_big_hunt_weekly_status SET latest_version = OLD.latest_version + 1 WHERE i_user_big_hunt_weekly_status.rowid = NEW.rowid;
END;

CREATE TABLE i_user_cage_ornament_reward (
  user_id INTEGER NOT NULL,
  cage_ornament_id integer NOT NULL,
  acquisition_datetime timestamp NOT NULL,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, cage_ornament_id)
);
CREATE INDEX i_user_cage_ornament_reward_user_id ON i_user_cage_ornament_reward(user_id);

CREATE TRIGGER i_user_cage_ornament_reward_update_version
AFTER UPDATE ON i_user_cage_ornament_reward FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_cage_ornament_reward SET latest_version = OLD.latest_version + 1 WHERE i_user_cage_ornament_reward.rowid = NEW.rowid;
END;

CREATE TABLE i_user_character (
  user_id INTEGER NOT NULL,
  character_id integer NOT NULL,
  level integer NOT NULL DEFAULT 1,
  exp integer NOT NULL DEFAULT 0,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, character_id)
);
CREATE INDEX i_user_character_user_id ON i_user_character(user_id);

CREATE TRIGGER i_user_character_update_version
AFTER UPDATE ON i_user_character FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_character SET latest_version = OLD.latest_version + 1 WHERE i_user_character.rowid = NEW.rowid;
END;

CREATE TABLE i_user_character_board (
  user_id INTEGER NOT NULL,
  character_board_id integer NOT NULL,
  panel_release_bit1 integer,
  panel_release_bit2 integer,
  panel_release_bit3 integer,
  panel_release_bit4 integer,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, character_board_id)
);
CREATE INDEX i_user_character_board_user_id ON i_user_character_board(user_id);

CREATE TRIGGER i_user_character_board_update_version
AFTER UPDATE ON i_user_character_board FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_character_board SET latest_version = OLD.latest_version + 1 WHERE i_user_character_board.rowid = NEW.rowid;
END;

CREATE TABLE i_user_character_board_ability (
  user_id INTEGER NOT NULL,
  character_id integer NOT NULL,
  ability_id integer NOT NULL,
  level integer,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, character_id, ability_id)
);
CREATE INDEX i_user_character_board_ability_user_id ON i_user_character_board_ability(user_id);

CREATE TRIGGER i_user_character_board_ability_update_version
AFTER UPDATE ON i_user_character_board_ability FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_character_board_ability SET latest_version = OLD.latest_version + 1 WHERE i_user_character_board_ability.rowid = NEW.rowid;
END;

CREATE TABLE i_user_character_board_complete_reward (
  user_id INTEGER NOT NULL,
  character_board_complete_reward_id integer NOT NULL,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, character_board_complete_reward_id)
);
CREATE INDEX i_user_character_board_complete_reward_user_id ON i_user_character_board_complete_reward(user_id);

CREATE TRIGGER i_user_character_board_complete_reward_update_version
AFTER UPDATE ON i_user_character_board_complete_reward FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_character_board_complete_reward SET latest_version = OLD.latest_version + 1 WHERE i_user_character_board_complete_reward.rowid = NEW.rowid;
END;

CREATE TABLE i_user_character_board_status_up (
  user_id INTEGER NOT NULL,
  character_id integer NOT NULL,
  status_calculation_type integer,
  hp integer,
  attack integer,
  vitality integer,
  agility integer,
  critical_ratio integer,
  critical_attack integer,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, character_id)
);
CREATE INDEX i_user_character_board_status_up_user_id ON i_user_character_board_status_up (user_id);

CREATE TRIGGER i_user_character_board_status_up_update_version
AFTER UPDATE ON i_user_character_board_status_up FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_character_board_status_up SET latest_version = OLD.latest_version + 1 WHERE i_user_character_board_status_up.rowid = NEW.rowid;
END;

CREATE TABLE i_user_character_costume_level_bonus (
  user_id INTEGER NOT NULL,
  character_id integer NOT NULL,
  status_calculation_type integer,
  hp integer,
  attack integer,
  vitality integer,
  agility integer,
  critical_ratio integer,
  critical_attack integer,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, character_id)
);
CREATE INDEX i_user_character_costume_level_bonus_user_id ON i_user_character_costume_level_bonus(user_id);

CREATE TRIGGER i_user_character_costume_level_bonus_update_version
AFTER UPDATE ON i_user_character_costume_level_bonus FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_character_costume_level_bonus SET latest_version = OLD.latest_version + 1 WHERE i_user_character_costume_level_bonus.rowid = NEW.rowid;
END;

CREATE TABLE i_user_character_rebirth (
  user_id INTEGER NOT NULL,
  character_id integer NOT NULL,
  rebirth_count integer,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, character_id)
);
CREATE INDEX i_user_character_rebirth_user_id ON i_user_character_rebirth(user_id);

CREATE TRIGGER i_user_character_rebirth_bonus_update_version
AFTER UPDATE ON i_user_character_rebirth FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_character_rebirth SET latest_version = OLD.latest_version + 1 WHERE i_user_character_rebirth.rowid = NEW.rowid;
END;

CREATE TABLE i_user_character_viewer_field (
  user_id INTEGER NOT NULL,
  character_viewer_field_id integer NOT NULL,
  release_datetime timestamp,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, character_viewer_field_id)
);
CREATE INDEX i_user_character_viewer_field_user_id ON i_user_character_viewer_field(user_id);

CREATE TRIGGER i_user_character_viewer_field_update_version
AFTER UPDATE ON i_user_character_viewer_field FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_character_viewer_field SET latest_version = OLD.latest_version + 1 WHERE i_user_character_viewer_field.rowid = NEW.rowid;
END;

CREATE TABLE i_user_comeback_campaign (
  user_id INTEGER NOT NULL,
  comeback_campaign_id integer NOT NULL,
  comeback_datetime timestamp,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, comeback_campaign_id)
);
CREATE INDEX i_user_comeback_campaign_user_id ON i_user_comeback_campaign(user_id);

CREATE TRIGGER i_user_comeback_campaign_update_version
AFTER UPDATE ON i_user_comeback_campaign FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_comeback_campaign SET latest_version = OLD.latest_version + 1 WHERE i_user_comeback_campaign.rowid = NEW.rowid;
END;

CREATE TABLE i_user_companion (
  user_id INTEGER NOT NULL,
  user_companion_uuid text NOT NULL,
  companion_id integer NOT NULL,
  headup_display_view_id integer NOT NULL DEFAULT 1, -- probably comes from USER_COMPANION_DEFAULT_HEADUP_DISPLAY_ID
  level integer NOT NULL DEFAULT 1,
  acquisition_datetime timestamp NOT NULL,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, user_companion_uuid)
);
CREATE INDEX i_user_companion_user_id ON i_user_companion(user_id);

CREATE TRIGGER i_user_companion_update_version
AFTER UPDATE ON i_user_companion FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_companion SET latest_version = OLD.latest_version + 1 WHERE i_user_companion.rowid = NEW.rowid;
END;

CREATE TABLE i_user_consumable_item (
  user_id INTEGER NOT NULL,
  consumable_item_id integer NOT NULL,
  count integer NOT NULL DEFAULT 0,
  first_acquisition_datetime timestamp NOT NULL,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, consumable_item_id)
);
CREATE INDEX i_user_consumable_item_user_id ON i_user_consumable_item(user_id);

CREATE TRIGGER i_user_consumable_item_update_version
AFTER UPDATE ON i_user_consumable_item FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_consumable_item SET latest_version = OLD.latest_version + 1 WHERE i_user_consumable_item.rowid = NEW.rowid;
END;

CREATE TABLE i_user_contents_story (
  user_id INTEGER NOT NULL,
  contents_story_id integer NOT NULL,
  play_datetime timestamp NOT NULL,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, contents_story_id)
);
CREATE INDEX i_user_contents_story_user_id ON i_user_contents_story(user_id);

CREATE TRIGGER i_user_contents_story_update_version
AFTER UPDATE ON i_user_contents_story FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_contents_story SET latest_version = OLD.latest_version + 1 WHERE i_user_contents_story.rowid = NEW.rowid;
END;

CREATE TABLE i_user_costume (
  user_id INTEGER NOT NULL,
  user_costume_uuid text NOT NULL,
  costume_id integer NOT NULL,
  limit_break_count integer NOT NULL DEFAULT 0,
  level integer NOT NULL DEFAULT 1,
  exp integer NOT NULL DEFAULT 0,
  headup_display_view_id integer NOT NULL DEFAULT 1, -- probably comes from USER_COSTUME_DEFAULT_HEADUP_DISPLAY_ID
  acquisition_datetime timestamp NOT NULL,
  awaken_count integer NOT NULL DEFAULT 0,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, costume_id)
);
CREATE INDEX i_user_costume_user_id ON i_user_costume(user_id);

CREATE TRIGGER i_user_costume_update_version
AFTER UPDATE ON i_user_costume FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_costume SET latest_version = OLD.latest_version + 1 WHERE i_user_costume.rowid = NEW.rowid;
END;

CREATE TABLE i_user_costume_active_skill (
  user_id INTEGER NOT NULL,
  user_costume_uuid text NOT NULL,
  level integer NOT NULL DEFAULT 1,
  acquisition_datetime timestamp NOT NULL,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, user_costume_uuid)
);
CREATE INDEX i_user_costume_active_skill_user_id ON i_user_costume_active_skill(user_id);

CREATE TRIGGER i_user_costume_active_skill_update_version
AFTER UPDATE ON i_user_costume_active_skill FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_costume_active_skill SET latest_version = OLD.latest_version + 1 WHERE i_user_costume_active_skill.rowid = NEW.rowid;
END;

CREATE TABLE i_user_costume_awaken_status_up (
  user_id INTEGER NOT NULL,
  user_costume_uuid text NOT NULL,
  status_calculation_type integer,
  hp integer,
  attack integer,
  vitality integer,
  agility integer,
  critical_ratio integer,
  critical_attack integer,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, user_costume_uuid)
);
CREATE INDEX i_user_costume_awaken_status_up_user_id ON i_user_costume_awaken_status_up(user_id);

CREATE TRIGGER i_user_costume_awaken_status_up_update_version
AFTER UPDATE ON i_user_costume_awaken_status_up FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_costume_awaken_status_up SET latest_version = OLD.latest_version + 1 WHERE i_user_costume_awaken_status_up.rowid = NEW.rowid;
END;

CREATE TABLE i_user_costume_level_bonus_release_status (
  user_id INTEGER NOT NULL,
  costume_id integer NOT NULL,
  last_released_bonus_level integer NOT NULL DEFAULT 0,
  confirmed_bonus_level integer NOT NULL DEFAULT 0,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, costume_id)
);
CREATE INDEX i_user_costume_level_bonus_release_status_user_id ON i_user_costume_level_bonus_release_status(user_id);

CREATE TRIGGER i_user_costume_level_bonus_release_status_update_version
AFTER UPDATE ON i_user_costume_level_bonus_release_status FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_costume_level_bonus_release_status SET latest_version = OLD.latest_version + 1 WHERE i_user_costume_level_bonus_release_status.rowid = NEW.rowid;
END;

CREATE TABLE i_user_costume_lottery_effect (
  user_id INTEGER NOT NULL,
  user_costume_uuid text NOT NULL,
  slot_number integer,
  odds_number integer,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, user_costume_uuid)
);
CREATE INDEX i_user_costume_lottery_effect_user_id ON i_user_costume_lottery_effect(user_id);

CREATE TRIGGER i_user_costume_lottery_effect_update_version
AFTER UPDATE ON i_user_costume_lottery_effect FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_costume_lottery_effect SET latest_version = OLD.latest_version + 1 WHERE i_user_costume_lottery_effect.rowid = NEW.rowid;
END;

CREATE TABLE i_user_costume_lottery_effect_ability (
  user_id INTEGER NOT NULL,
  user_costume_uuid text NOT NULL,
  slot_number integer,
  ability_id integer,
  ability_level integer,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, user_costume_uuid)
);
CREATE INDEX i_user_costume_lottery_effect_ability_user_id ON i_user_costume_lottery_effect_ability(user_id);

CREATE TRIGGER i_user_costume_lottery_effect_ability_update_version
AFTER UPDATE ON i_user_costume_lottery_effect_ability FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_costume_lottery_effect_ability SET latest_version = OLD.latest_version + 1 WHERE i_user_costume_lottery_effect_ability.rowid = NEW.rowid;
END;

CREATE TABLE i_user_costume_lottery_effect_pending (
  user_id INTEGER NOT NULL,
  user_costume_uuid text NOT NULL,
  slot_number integer,
  odds_number integer,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, user_costume_uuid)
);
CREATE INDEX i_user_costume_lottery_effect_pending_user_id ON i_user_costume_lottery_effect_pending(user_id);

CREATE TRIGGER i_user_costume_lottery_effect_pending_update_version
AFTER UPDATE ON i_user_costume_lottery_effect_pending FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_costume_lottery_effect_pending SET latest_version = OLD.latest_version + 1 WHERE i_user_costume_lottery_effect_pending.rowid = NEW.rowid;
END;

CREATE TABLE i_user_costume_lottery_effect_status_up (
  user_id INTEGER NOT NULL,
  user_costume_uuid text NOT NULL,
  status_calculation_type integer,
  hp integer,
  attack integer,
  vitality integer,
  agility integer,
  critical_ratio integer,
  critical_attack integer,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, user_costume_uuid)
);
CREATE INDEX i_user_costume_lottery_effect_status_up_user_id ON i_user_costume_lottery_effect_status_up(user_id);

CREATE TRIGGER i_user_costume_lottery_effect_status_up_update_version
AFTER UPDATE ON i_user_costume_lottery_effect_status_up FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_costume_lottery_effect_status_up SET latest_version = OLD.latest_version + 1 WHERE i_user_costume_lottery_effect_status_up.rowid = NEW.rowid;
END;

CREATE TABLE i_user_deck (
  user_id INTEGER NOT NULL,
  deck_type integer NOT NULL DEFAULT 1,
  user_deck_number integer NOT NULL DEFAULT 1,
  user_deck_character_uuid01 text NOT NULL DEFAULT '',
  user_deck_character_uuid02 text NOT NULL DEFAULT '',
  user_deck_character_uuid03 text NOT NULL DEFAULT '',
  name text NOT NULL DEFAULT '',
  power integer NOT NULL DEFAULT 0,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, deck_type, user_deck_number)
);
CREATE INDEX i_user_deck_user_id ON i_user_deck(user_id);

CREATE TRIGGER i_user_deck_update_version
AFTER UPDATE ON i_user_deck FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_deck SET latest_version = OLD.latest_version + 1 WHERE i_user_deck.rowid = NEW.rowid;
END;

CREATE TABLE i_user_deck_character (
  user_id INTEGER NOT NULL,
  user_deck_character_uuid text NOT NULL,
  user_costume_uuid text NOT NULL DEFAULT '',
  main_user_weapon_uuid text NOT NULL DEFAULT '',
  user_companion_uuid text NOT NULL DEFAULT '',
  power integer NOT NULL DEFAULT 0,
  user_thought_uuid text NOT NULL DEFAULT '',
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, user_deck_character_uuid)
);
CREATE INDEX i_user_deck_character_user_id ON i_user_deck_character(user_id);

CREATE TRIGGER i_user_deck_character_update_version
AFTER UPDATE ON i_user_deck_character FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_deck_character SET latest_version = OLD.latest_version + 1 WHERE i_user_deck_character.rowid = NEW.rowid;
END;

CREATE TABLE i_user_deck_character_dressup_costume (
  user_id INTEGER NOT NULL,
  user_deck_character_uuid text NOT NULL,
  dressup_costume_id integer NOT NULL,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, user_deck_character_uuid)
);
CREATE INDEX i_user_deck_character_dressup_costume_user_id ON i_user_deck_character_dressup_costume(user_id);

CREATE TRIGGER i_user_deck_character_dressup_costume_update_version
AFTER UPDATE ON i_user_deck_character_dressup_costume FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_deck_character_dressup_costume SET latest_version = OLD.latest_version + 1 WHERE i_user_deck_character_dressup_costume.rowid = NEW.rowid;
END;

CREATE TABLE i_user_deck_limit_content_restricted (
  user_id INTEGER NOT NULL,
  event_quest_chapter_id integer NOT NULL,
  quest_id integer NOT NULL,
  deck_restricted_uuid text,
  possession_type integer,
  target_uuid text,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, event_quest_chapter_id, quest_id)
);
CREATE INDEX i_user_deck_limit_content_restricted_user_id ON i_user_deck_limit_content_restricted(user_id);

CREATE TRIGGER i_user_deck_limit_content_restricted_update_version
AFTER UPDATE ON i_user_deck_limit_content_restricted FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_deck_limit_content_restricted SET latest_version = OLD.latest_version + 1 WHERE i_user_deck_limit_content_restricted.rowid = NEW.rowid;
END;

CREATE TABLE i_user_deck_parts_group (
  user_id INTEGER NOT NULL,
  user_deck_character_uuid text NOT NULL,
  user_parts_uuid text NOT NULL,
  sort_order integer NOT NULL,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, user_deck_character_uuid, user_parts_uuid)
);
CREATE INDEX i_user_deck_parts_group_user_id ON i_user_deck_parts_group(user_id);

CREATE TRIGGER i_user_deck_parts_group_update_version
AFTER UPDATE ON i_user_deck_parts_group FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_deck_parts_group SET latest_version = OLD.latest_version + 1 WHERE i_user_deck_parts_group.rowid = NEW.rowid;
END;

CREATE TABLE i_user_deck_sub_weapon_group (
  user_id INTEGER NOT NULL,
  user_deck_character_uuid text NOT NULL,
  user_weapon_uuid text NOT NULL,
  sort_order integer NOT NULL,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, user_deck_character_uuid, user_weapon_uuid)
);
CREATE INDEX i_user_deck_sub_weapon_group_user_id ON i_user_deck_sub_weapon_group(user_id);

CREATE TRIGGER i_user_deck_sub_weapon_group_update_version
AFTER UPDATE ON i_user_deck_sub_weapon_group FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_deck_sub_weapon_group SET latest_version = OLD.latest_version + 1 WHERE i_user_deck_sub_weapon_group.rowid = NEW.rowid;
END;

CREATE TABLE i_user_deck_type_note (
  user_id INTEGER NOT NULL,
  deck_type integer NOT NULL,
  max_deck_power integer NOT NULL DEFAULT 0,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, deck_type)
);
CREATE INDEX i_user_deck_type_note_user_id ON i_user_deck_type_note(user_id);

CREATE TRIGGER i_user_deck_type_note_update_version
AFTER UPDATE ON i_user_deck_type_note FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_deck_type_note SET latest_version = OLD.latest_version + 1 WHERE i_user_deck_type_note.rowid = NEW.rowid;
END;

CREATE TABLE i_user_dokan (
  user_id INTEGER NOT NULL,
  dokan_id integer NOT NULL,
  display_datetime timestamp NOT NULL,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, dokan_id)
);
CREATE INDEX i_user_dokan_user_id ON i_user_dokan(user_id);

CREATE TRIGGER i_user_dokan_update_version
AFTER UPDATE ON i_user_dokan FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_dokan SET latest_version = OLD.latest_version + 1 WHERE i_user_dokan.rowid = NEW.rowid;
END;

CREATE TABLE i_user_event_quest_daily_group_complete_reward (
  user_id INTEGER NOT NULL PRIMARY KEY,
  last_reward_receive_event_quest_daily_group_id integer,
  last_reward_receive_datetime timestamp,
  latest_version bigint NOT NULL DEFAULT 1
);

CREATE TRIGGER i_user_event_quest_daily_group_complete_reward_update_version
AFTER UPDATE ON i_user_event_quest_daily_group_complete_reward FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_event_quest_daily_group_complete_reward SET latest_version = OLD.latest_version + 1
    WHERE i_user_event_quest_daily_group_complete_reward.rowid = NEW.rowid;
END;

CREATE TABLE i_user_event_quest_guerrilla_free_open (
  user_id INTEGER NOT NULL PRIMARY KEY,
  start_datetime timestamp,
  open_minutes integer,
  daily_opened_count integer,
  latest_version bigint NOT NULL DEFAULT 1
);

CREATE TRIGGER i_user_event_quest_guerrilla_free_open_update_version
AFTER UPDATE ON i_user_event_quest_guerrilla_free_open FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_event_quest_guerrilla_free_open SET latest_version = OLD.latest_version + 1
    WHERE i_user_event_quest_guerrilla_free_open.rowid = NEW.rowid;
END;

CREATE TABLE i_user_event_quest_labyrinth_season (
  user_id INTEGER NOT NULL PRIMARY KEY,
  event_quest_chapter_id integer,
  last_join_season_number integer,
  last_season_reward_received_season_number integer,
  latest_version bigint NOT NULL DEFAULT 1
);

CREATE TRIGGER i_user_event_quest_labyrinth_season_update_version
AFTER UPDATE ON i_user_event_quest_labyrinth_season FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_event_quest_labyrinth_season SET latest_version = OLD.latest_version + 1
    WHERE i_user_event_quest_labyrinth_season.rowid = NEW.rowid;
END;

CREATE TABLE i_user_event_quest_labyrinth_stage (
  user_id INTEGER NOT NULL PRIMARY KEY,
  event_quest_chapter_id integer,
  stage_order integer,
  is_received_stage_clear_reward boolean,
  accumulation_reward_received_quest_mission_count integer,
  latest_version bigint NOT NULL DEFAULT 1
);

CREATE TRIGGER i_user_event_quest_labyrinth_stage_update_version
AFTER UPDATE ON i_user_event_quest_labyrinth_stage FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_event_quest_labyrinth_stage SET latest_version = OLD.latest_version + 1
    WHERE i_user_event_quest_labyrinth_stage.rowid = NEW.rowid;
END;

CREATE TABLE i_user_event_quest_progress_status (
  user_id INTEGER NOT NULL PRIMARY KEY,
  current_event_quest_chapter_id integer,
  current_quest_id integer,
  current_quest_scene_id integer,
  head_quest_scene_id integer,
  latest_version bigint NOT NULL DEFAULT 1
);

CREATE TRIGGER i_user_event_quest_progress_status_update_version
AFTER UPDATE ON i_user_event_quest_progress_status FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_event_quest_progress_status SET latest_version = OLD.latest_version + 1
    WHERE i_user_event_quest_progress_status.rowid = NEW.rowid;
END;

CREATE TABLE i_user_event_quest_tower_accumulation_reward (
  user_id INTEGER NOT NULL,
  event_quest_chapter_id integer NOT NULL,
  latest_reward_receive_quest_mission_clear_count integer,
  latest_version bigin NOT NULL DEFAULT 1
);
CREATE INDEX i_user_event_quest_tower_accumulation_reward_user_id ON i_user_event_quest_tower_accumulation_reward(user_id);

CREATE TRIGGER i_user_event_quest_tower_accumulation_reward_update_version
AFTER UPDATE ON i_user_event_quest_tower_accumulation_reward FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_event_quest_tower_accumulation_reward SET latest_version = OLD.latest_version + 1
    WHERE i_user_event_quest_tower_accumulation_reward.rowid = NEW.rowid;
END;

CREATE TABLE i_user_explore (
  user_id INTEGER NOT NULL PRIMARY KEY,
  is_use_explore_ticket boolean,
  playing_explore_id integer,
  latest_play_datetime timestamp,
  latest_version bigint NOT NULL DEFAULT 1
);

CREATE TRIGGER i_user_explore_update_version
AFTER UPDATE ON i_user_explore FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_explore SET latest_version = OLD.latest_version + 1
    WHERE i_user_explore.rowid = NEW.rowid;
END;

CREATE TABLE i_user_explore_score (
  user_id INTEGER NOT NULL,
  explore_id integer NOT NULL,
  max_score integer,
  max_score_update_datetime timestamp,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, explore_id)
);
CREATE INDEX i_user_explore_score_user_id ON i_user_explore_score(user_id);

CREATE TRIGGER i_user_explore_score_update_version
AFTER UPDATE ON i_user_explore_score FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_explore_score SET latest_version = OLD.latest_version + 1
    WHERE i_user_explore_score.rowid = NEW.rowid;
END;

CREATE TABLE i_user_extra_quest_progress_status (
  user_id INTEGER NOT NULL PRIMARY KEY,
  current_quest_id integer,
  current_quest_scene_id integer,
  head_quest_scene_id integer,
  latest_version bigint NOT NULL DEFAULT 1
);

CREATE TRIGGER i_user_extra_quest_progress_status_update_version
AFTER UPDATE ON i_user_extra_quest_progress_status FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_extra_quest_progress_status SET latest_version = OLD.latest_version + 1
    WHERE i_user_extra_quest_progress_status.rowid = NEW.rowid;
END;

CREATE TABLE i_user_facebook (
  user_id INTEGER NOT NULL PRIMARY KEY,
  facebook_id bigint,
  latest_version bigint NOT NULL DEFAULT 1
);

CREATE TRIGGER i_user_facebook_update_version
AFTER UPDATE ON i_user_facebook FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_facebook SET latest_version = OLD.latest_version + 1
    WHERE i_user_facebook.rowid = NEW.rowid;
END;

CREATE TABLE i_user_gem (
  user_id INTEGER NOT NULL PRIMARY KEY,
  paid_gem integer NOT NULL DEFAULT 0,
  free_gem integer NOT NULL DEFAULT 0
);

CREATE TABLE i_user_gimmick (
  user_id INTEGER NOT NULL,
  gimmick_sequence_schedule_id integer NOT NULL,
  gimmick_sequence_id integer NOT NULL,
  gimmick_id integer NOT NULL,
  is_gimmick_cleared boolean,
  start_datetime timestamp,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, gimmick_sequence_schedule_id, gimmick_sequence_id, gimmick_id)
);

CREATE INDEX i_user_gimmick_user_id ON i_user_gimmick(user_id);

CREATE TRIGGER i_user_gimmick_update_version
AFTER UPDATE ON i_user_gimmick FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_gimmick SET latest_version = OLD.latest_version + 1
    WHERE i_user_gimmick.rowid = NEW.rowid;
END;

CREATE TABLE i_user_gimmick_ornament_progress (
  user_id INTEGER NOT NULL,
  gimmick_sequence_schedule_id integer NOT NULL,
  gimmick_sequence_id integer NOT NULL,
  gimmick_id integer NOT NULL,
  gimmick_ornament_index integer NOT NULL,
  progress_value_bit integer,
  base_datetime timestamp,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, gimmick_sequence_schedule_id, gimmick_sequence_id, gimmick_id, gimmick_ornament_index)
);

CREATE INDEX i_user_gimmick_ornament_progress_user_id ON i_user_gimmick_ornament_progress(user_id);

CREATE TRIGGER i_user_gimmick_ornament_progress_update_version
AFTER UPDATE ON i_user_gimmick_ornament_progress FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_gimmick_ornament_progress SET latest_version = OLD.latest_version + 1
    WHERE i_user_gimmick_ornament_progress.rowid = NEW.rowid;
END;

CREATE TABLE i_user_gimmick_sequence (
  user_id INTEGER NOT NULL,
  gimmick_sequence_schedule_id integer NOT NULL,
  gimmick_sequence_id integer NOT NULL,
  is_gimmick_sequence_cleared boolean,
  clear_datetime timestamp,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, gimmick_sequence_schedule_id, gimmick_sequence_id)
);

CREATE INDEX i_user_gimmick_sequence_user_id ON i_user_gimmick_sequence(user_id);

CREATE TRIGGER i_user_gimmick_sequence_update_version
AFTER UPDATE ON i_user_gimmick_sequence FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_gimmick_sequence SET latest_version = OLD.latest_version + 1
    WHERE i_user_gimmick_sequence.rowid = NEW.rowid;
END;

CREATE TABLE i_user_gimmick_unlock (
  user_id INTEGER NOT NULL,
  gimmick_sequence_schedule_id integer NOT NULL,
  gimmick_sequence_id integer NOT NULL,
  gimmick_id integer NOT NULL,
  is_unlocked boolean,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, gimmick_sequence_schedule_id, gimmick_sequence_id, gimmick_id)
);

CREATE INDEX i_user_gimmick_unlock_user_id ON i_user_gimmick_unlock(user_id);

CREATE TRIGGER i_user_gimmick_unlock_update_version
AFTER UPDATE ON i_user_gimmick_unlock FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_gimmick_unlock SET latest_version = OLD.latest_version + 1
    WHERE i_user_gimmick_unlock.rowid = NEW.rowid;
END;

CREATE TABLE i_user_important_item (
  user_id INTEGER NOT NULL,
  important_item_id integer NOT NULL,
  count integer NOT NULL,
  first_acquisition_datetime timestamp NOT NULL,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, important_item_id)
);

CREATE INDEX i_user_important_item_user_id ON i_user_important_item(user_id);

CREATE TRIGGER i_user_important_item_update_version
AFTER UPDATE ON i_user_important_item FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_important_item SET latest_version = OLD.latest_version + 1
    WHERE i_user_important_item.rowid = NEW.rowid;
END;

CREATE TABLE i_user_limited_open (
  user_id INTEGER NOT NULL,
  limited_open_target_type integer NOT NULL,
  target_id integer NOT NULL,
  open_datetime timestamp,
  close_datetime timestamp,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, limited_open_target_type, target_id)
);

CREATE INDEX i_user_limited_open_user_id ON i_user_limited_open(user_id);

CREATE TRIGGER i_user_limited_open_update_version
AFTER UPDATE ON i_user_limited_open FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_limited_open SET latest_version = OLD.latest_version + 1
    WHERE i_user_limited_open.rowid = NEW.rowid;
END;

CREATE TABLE i_user_login (
  user_id INTEGER NOT NULL PRIMARY KEY,
  total_login_count integer NOT NULL DEFAULT 0,
  continual_login_count integer NOT NULL DEFAULT 0,
  max_continual_login_count integer NOT NULL DEFAULT 0,
  last_login_datetime timestamp NOT NULL DEFAULT 28800000,
  last_comeback_login_datetime timestamp NOT NULL DEFAULT 28800000,
  latest_version bigint NOT NULL DEFAULT 1
);

CREATE TRIGGER i_user_login_update_version
AFTER UPDATE ON i_user_login FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_login SET latest_version = OLD.latest_version + 1
    WHERE i_user_login.rowid = NEW.rowid;
END;

CREATE TABLE i_user_login_bonus (
  user_id INTEGER NOT NULL,
  login_bonus_id integer NOT NULL,
  current_page_number integer,
  current_stamp_number integer,
  latest_reward_receive_datetime timestamp,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, login_bonus_id)
);

CREATE INDEX i_user_login_bonus_user_id ON i_user_login_bonus(user_id);

CREATE TRIGGER i_user_login_bonus_update_version
AFTER UPDATE ON i_user_login_bonus FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_login_bonus SET latest_version = OLD.latest_version + 1
    WHERE i_user_login_bonus.rowid = NEW.rowid;
END;

CREATE TABLE i_user_main_quest_flow_status (
  user_id INTEGER NOT NULL PRIMARY KEY,
  current_quest_flow_type integer,
  latest_version bigint NOT NULL DEFAULT 1
);

CREATE TRIGGER i_user_main_quest_flow_status_update_version
AFTER UPDATE ON i_user_main_quest_flow_status FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_main_quest_flow_status SET latest_version = OLD.latest_version + 1
    WHERE i_user_main_quest_flow_status.rowid = NEW.rowid;
END;

CREATE TABLE i_user_main_quest_main_flow_status (
  user_id INTEGER NOT NULL PRIMARY KEY,
  current_main_quest_route_id integer NOT NULL DEFAULT 1,
  current_quest_scene_id integer NOT NULL DEFAULT 0,
  head_quest_scene_id integer NOT NULL DEFAULT 0,
  is_reached_last_quest_scene boolean NOT NULL DEFAULT 0,
  latest_version bigint NOT NULL DEFAULT 1
);

CREATE TRIGGER i_user_main_quest_main_flow_status_update_version
AFTER UPDATE ON i_user_main_quest_main_flow_status FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_main_quest_main_flow_status SET latest_version = OLD.latest_version + 1
    WHERE i_user_main_quest_main_flow_status.rowid = NEW.rowid;
END;

CREATE TABLE i_user_main_quest_progress_status (
  user_id INTEGER NOT NULL PRIMARY KEY,
  current_quest_scene_id integer,
  head_quest_scene_id integer,
  current_quest_flow_type integer,
  latest_version bigint NOT NULL DEFAULT 1
);

CREATE TRIGGER i_user_main_quest_progress_status_update_version
AFTER UPDATE ON i_user_main_quest_progress_status FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_main_quest_progress_status SET latest_version = OLD.latest_version + 1
    WHERE i_user_main_quest_progress_status.rowid = NEW.rowid;
END;

CREATE TABLE i_user_main_quest_replay_flow_status (
  user_id INTEGER NOT NULL PRIMARY KEY,
  current_head_quest_scene_id integer,
  current_quest_scene_id integer,
  latest_version bigint NOT NULL DEFAULT 1
);

CREATE TRIGGER i_user_main_quest_replay_flow_status_update_version
AFTER UPDATE ON i_user_main_quest_replay_flow_status FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_main_quest_replay_flow_status SET latest_version = OLD.latest_version + 1
    WHERE i_user_main_quest_replay_flow_status.rowid = NEW.rowid;
END;

CREATE TABLE i_user_main_quest_season_route (
  user_id INTEGER NOT NULL,
  main_quest_season_id integer NOT NULL,
  main_quest_route_id integer NOT NULL,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, main_quest_season_id)
);

CREATE TRIGGER i_user_main_quest_season_route_update_version
AFTER UPDATE ON i_user_main_quest_season_route FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_main_quest_season_route SET latest_version = OLD.latest_version + 1
    WHERE i_user_main_quest_season_route.rowid = NEW.rowid;
END;

CREATE TABLE i_user_material (
  user_id INTEGER NOT NULL,
  material_id integer NOT NULL,
  count integer NOT NULL DEFAULT 0,
  first_acquisition_datetime timestamp NOT NULL,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, material_id)
);

CREATE INDEX i_user_material_user_id ON i_user_material(user_id);

CREATE TRIGGER i_user_material_update_version
AFTER UPDATE ON i_user_material FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_material SET latest_version = OLD.latest_version + 1
    WHERE i_user_material.rowid = NEW.rowid;
END;

CREATE TABLE i_user_mission (
  user_id INTEGER NOT NULL,
  mission_id integer NOT NULL,
  start_datetime timestamp,
  progress_value integer,
  mission_progress_status_type integer,
  clear_datetime timestamp,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, mission_id)
);

CREATE INDEX i_user_mission_user_id ON i_user_mission(user_id);

CREATE TRIGGER i_user_mission_update_version
AFTER UPDATE ON i_user_mission FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_mission SET latest_version = OLD.latest_version + 1
    WHERE i_user_mission.rowid = NEW.rowid;
END;

CREATE TABLE i_user_mission_completion_progress (
  user_id INTEGER NOT NULL,
  mission_id integer NOT NULL,
  progress_value bigint,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, mission_id)
);

CREATE INDEX i_user_mission_completion_progress_user_id ON i_user_mission_completion_progress(user_id);

CREATE TRIGGER i_user_mission_completion_progress_update_version
AFTER UPDATE ON i_user_mission_completion_progress FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_mission_completion_progress SET latest_version = OLD.latest_version + 1
    WHERE i_user_mission_completion_progress.rowid = NEW.rowid;
END;

CREATE TABLE i_user_mission_pass_point (
  user_id INTEGER NOT NULL,
  mission_pass_id integer NOT NULL,
  point integer NOT NULL,
  premium_reward_received_level integer,
  no_premium_reward_received_level integer,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, mission_pass_id, point)
);

CREATE INDEX i_user_mission_pass_point_user_id ON i_user_mission_pass_point(user_id);

CREATE TRIGGER i_user_mission_pass_point_update_version
AFTER UPDATE ON i_user_mission_pass_point FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_mission_pass_point SET latest_version = OLD.latest_version + 1
    WHERE i_user_mission_pass_point.rowid = NEW.rowid;
END;

CREATE TABLE i_user_movie (
  user_id INTEGER NOT NULL,
  movie_id integer NOT NULL,
  latest_viewed_datetime timestamp,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, movie_id)
);

CREATE INDEX i_user_movie_user_id ON i_user_movie(user_id);

CREATE TRIGGER i_user_movie_update_version
AFTER UPDATE ON i_user_movie FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_movie SET latest_version = OLD.latest_version + 1
    WHERE i_user_movie.rowid = NEW.rowid;
END;

CREATE TABLE i_user_navi_cut_in (
  user_id INTEGER NOT NULL,
  navi_cut_in_id integer NOT NULL,
  play_datetime timestamp NOT NULL,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, navi_cut_in_id)
);

CREATE INDEX i_user_navi_cut_in_id ON i_user_movie(user_id);

CREATE TRIGGER i_user_navi_cut_in_update_version
AFTER UPDATE ON i_user_navi_cut_in FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_navi_cut_in SET latest_version = OLD.latest_version + 1
    WHERE i_user_navi_cut_in.rowid = NEW.rowid;
END;

CREATE TABLE i_user_omikuji (
  user_id INTEGER NOT NULL,
  omikuji_id integer NOT NULL,
  latest_draw_datetime timestamp NOT NULL,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, omikuji_id)
);

CREATE INDEX i_user_omikuji_id ON i_user_omikuji(user_id);

CREATE TRIGGER i_user_omikuji_update_version
AFTER UPDATE ON i_user_omikuji FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_omikuji SET latest_version = OLD.latest_version + 1
    WHERE i_user_omikuji.rowid = NEW.rowid;
END;

CREATE TABLE i_user_parts (
  user_id INTEGER NOT NULL,
  user_parts_uuid text NOT NULL,
  parts_id integer NOT NULL,
  level integer NOT NULL DEFAULT 1,
  parts_status_main_id integer NOT NULL DEFAULT 0,
  is_protected boolean NOT NULL DEFAULT 0,
  acquisition_datetime timestamp NOT NULL,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, user_parts_uuid)
);

CREATE INDEX i_user_parts_id_user_id ON i_user_parts(user_id);

CREATE TRIGGER i_user_parts_update_version
AFTER UPDATE ON i_user_parts FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_parts SET latest_version = OLD.latest_version + 1
    WHERE i_user_parts.rowid = NEW.rowid;
END;

CREATE TABLE i_user_parts_group_note (
  user_id INTEGER NOT NULL,
  parts_group_id integer NOT NULL,
  first_acquisition_datetime timestamp,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, parts_group_id)
);

CREATE INDEX i_user_parts_group_note_user_id ON i_user_parts_group_note(user_id);

CREATE TRIGGER i_user_parts_group_note_update_version
AFTER UPDATE ON i_user_parts_group_note FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_parts_group_note SET latest_version = OLD.latest_version + 1
    WHERE i_user_parts_group_note.rowid = NEW.rowid;
END;

CREATE TABLE i_user_parts_preset (
  user_id INTEGER NOT NULL,
  user_parts_preset_number integer NOT NULL,
  user_parts_uuid01 text,
  user_parts_uuid02 text,
  user_parts_uuid03 text,
  name text,
  user_parts_preset_tag_number integer,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, user_parts_preset_number)
);

CREATE INDEX i_user_parts_preset_user_id ON i_user_parts_preset(user_id);

CREATE TRIGGER i_user_parts_preset_update_version
AFTER UPDATE ON i_user_parts_preset FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_parts_preset SET latest_version = OLD.latest_version + 1
    WHERE i_user_parts_preset.rowid = NEW.rowid;
END;

CREATE TABLE i_user_parts_preset_tag (
  user_id INTEGER NOT NULL,
  user_parts_preset_tag_number integer NOT NULL,
  name text,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, user_parts_preset_tag_number)
);

CREATE INDEX i_user_parts_preset_tag_user_id ON i_user_parts_preset_tag(user_id);

CREATE TRIGGER i_user_parts_preset_tag_update_version
AFTER UPDATE ON i_user_parts_preset_tag FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_parts_preset_tag SET latest_version = OLD.latest_version + 1
    WHERE i_user_parts_preset_tag.rowid = NEW.rowid;
END;

CREATE TABLE i_user_parts_status_sub (
  user_id INTEGER NOT NULL,
  user_parts_uuid text NOT NULL,
  status_index integer NOT NULL,
  parts_status_sub_lottery_id integer,
  level integer,
  status_kind_type integer,
  status_calculation_type integer,
  status_change_value integer,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, user_parts_uuid, status_index)
);

CREATE INDEX i_user_parts_status_sub_user_id ON i_user_parts_status_sub(user_id);

CREATE TRIGGER i_user_parts_status_sub_update_version
AFTER UPDATE ON i_user_parts_status_sub FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_parts_status_sub SET latest_version = OLD.latest_version + 1
    WHERE i_user_parts_status_sub.rowid = NEW.rowid;
END;

CREATE TABLE i_user_portal_cage_status (
  user_id INTEGER NOT NULL PRIMARY KEY,
  is_current_progress boolean NOT NULL DEFAULT 0,
  drop_item_start_datetime timestamp NOT NULL DEFAULT 0,
  current_drop_item_count integer NOT NULL DEFAULT 0,
  latest_version bigint NOT NULL DEFAULT 1
);

CREATE TRIGGER i_user_portal_cage_status_update_version
AFTER UPDATE ON i_user_portal_cage_status FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_portal_cage_status SET latest_version = OLD.latest_version + 1
    WHERE i_user_portal_cage_status.rowid = NEW.rowid;
END;

CREATE TABLE i_user_possession_auto_convert (
  user_id INTEGER NOT NULL,
  possession_type integer NOT NULL,
  possession_id integer NOT NULL,
  from_count integer,
  to_possession_type integer,
  to_possession_id integer,
  to_count integer,
  convert_datetime timestamp,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, possession_type, possession_id)
);

CREATE INDEX i_user_possession_auto_convert_user_id ON i_user_possession_auto_convert(user_id);

CREATE TRIGGER i_user_possession_auto_convert_update_version
AFTER UPDATE ON i_user_possession_auto_convert FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_possession_auto_convert SET latest_version = OLD.latest_version + 1
    WHERE i_user_possession_auto_convert.rowid = NEW.rowid;
END;

CREATE TABLE i_user_premium_item (
  user_id INTEGER NOT NULL,
  premium_item_id integer NOT NULL,
  acquisition_datetime timestamp,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, premium_item_id)
);

CREATE INDEX i_user_premium_item_user_id ON i_user_premium_item(user_id);

CREATE TRIGGER i_user_premium_item_update_version
AFTER UPDATE ON i_user_premium_item FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_premium_item SET latest_version = OLD.latest_version + 1
    WHERE i_user_premium_item.rowid = NEW.rowid;
END;

CREATE TABLE i_user_profile (
  user_id INTEGER NOT NULL PRIMARY KEY,
  name text NOT NULL DEFAULT '',
  name_update_datetime timestamp NOT NULL DEFAULT 28800000,
  message text NOT NULL DEFAULT '',
  message_update_datetime timestamp NOT NULL DEFAULT 28800000,
  favorite_costume_id integer NOT NULL DEFAULT 0,
  favorite_costume_id_update_datetime timestamp NOT NULL DEFAULT 28800000,
  latest_version bigint NOT NULL DEFAULT 1
);

CREATE TRIGGER i_user_profile_update_version
AFTER UPDATE ON i_user_profile FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_profile SET latest_version = OLD.latest_version + 1
    WHERE i_user_profile.rowid = NEW.rowid;
END;

CREATE TRIGGER i_user_profile_update_name
AFTER UPDATE OF name ON i_user_profile FOR EACH ROW
BEGIN
  UPDATE i_user_profile SET name_update_datetime = current_net_timestamp() WHERE rowid = NEW.rowid;
END;

CREATE TRIGGER i_user_profile_update_message
AFTER UPDATE OF message ON i_user_profile FOR EACH ROW
BEGIN
  UPDATE i_user_profile SET message_update_datetime = current_net_timestamp() WHERE rowid = NEW.rowid;
END;

CREATE TRIGGER i_user_profile_update_favorite_costume
AFTER UPDATE OF favorite_costume_id ON i_user_profile FOR EACH ROW
BEGIN
  UPDATE i_user_profile SET favorite_costume_id_update_datetime = current_net_timestamp() WHERE rowid = NEW.rowid;
END;

CREATE TABLE i_user_pvp_defense_deck (
  user_id INTEGER NOT NULL PRIMARY KEY,
  user_deck_number integer,
  latest_version bigint NOT NULL DEFAULT 1
);

CREATE TRIGGER i_user_pvp_defense_deck_update_version
AFTER UPDATE ON i_user_pvp_defense_deck FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_pvp_defense_deck SET latest_version = OLD.latest_version + 1
    WHERE i_user_pvp_defense_deck.rowid = NEW.rowid;
END;

CREATE TABLE i_user_pvp_status (
  user_id INTEGER NOT NULL PRIMARY KEY,
  stamina_milli_value integer,
  stamina_update_datetime timestamp,
  latest_reward_receive_pvp_season_id integer,
  latest_reward_receive_pvp_weekly_version bigint,
  win_streak_count integer,
  win_streak_count_update_datetime timestamp,
  latest_version bigint NOT NULL DEFAULT 1
);

CREATE TRIGGER i_user_pvp_status_update_version
AFTER UPDATE ON i_user_pvp_status FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_pvp_status SET latest_version = OLD.latest_version + 1
    WHERE i_user_pvp_status.rowid = NEW.rowid;
END;

CREATE TABLE i_user_pvp_weekly_result (
  user_id INTEGER NOT NULL,
  pvp_weekly_version bigint NOT NULL,
  pvp_season_id integer,
  group_id integer,
  final_point integer,
  final_rank integer,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, pvp_weekly_version)
);
CREATE INDEX i_user_pvp_weekly_result_user_id ON i_user_pvp_weekly_result(user_id);

CREATE TRIGGER i_user_pvp_weekly_result_update_version
AFTER UPDATE ON i_user_pvp_weekly_result FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_pvp_weekly_result SET latest_version = OLD.latest_version + 1
    WHERE i_user_pvp_weekly_result.rowid = NEW.rowid;
END;

CREATE TABLE i_user_quest (
  user_id INTEGER NOT NULL,
  quest_id integer NOT NULL,
  quest_state_type integer NOT NULL DEFAULT 0,
  is_battle_only boolean NOT NULL DEFAULT 0,
  latest_start_datetime timestamp NOT NULL DEFAULT 28800000,
  clear_count integer NOT NULL DEFAULT 0,
  daily_clear_count integer NOT NULL DEFAULT 0,
  last_clear_datetime timestamp NOT NULL DEFAULT 28800000,
  shortest_clear_frames integer NOT NULL DEFAULT 2147483647,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, quest_id)
);
CREATE INDEX i_user_quest_user_id ON i_user_quest(user_id);

CREATE TRIGGER i_user_quest_update_version
AFTER UPDATE ON i_user_quest FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_quest SET latest_version = OLD.latest_version + 1
    WHERE i_user_quest.rowid = NEW.rowid;
END;

CREATE TABLE i_user_quest_auto_orbit (
  user_id INTEGER NOT NULL PRIMARY KEY,
  quest_type integer,
  chapter_id integer,
  quest_id integer,
  max_auto_orbit_count integer,
  cleared_auto_orbit_count integer,
  last_clear_datetime timestamp,
  latest_version bigint NOT NULL DEFAULT 1
);
CREATE INDEX i_user_quest_auto_orbit_user_id ON i_user_quest_auto_orbit(user_id);

CREATE TRIGGER i_user_quest_auto_orbit_update_version
AFTER UPDATE ON i_user_quest_auto_orbit FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_quest_auto_orbit SET latest_version = OLD.latest_version + 1
    WHERE i_user_quest_auto_orbit.rowid = NEW.rowid;
END;

CREATE TABLE i_user_quest_limit_content_status (
  user_id INTEGER NOT NULL,
  quest_id integer NOT NULL,
  limit_content_quest_status_type integer,
  event_quest_chapter_id integer,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, quest_id)
);
CREATE INDEX i_user_quest_limit_content_status_user_id ON i_user_quest_limit_content_status(user_id);

CREATE TRIGGER i_user_quest_limit_content_status_update_version
AFTER UPDATE ON i_user_quest_limit_content_status FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_quest_limit_content_status SET latest_version = OLD.latest_version + 1
    WHERE i_user_quest_limit_content_status.rowid = NEW.rowid;
END;

CREATE TABLE i_user_quest_mission (
  user_id INTEGER NOT NULL,
  quest_id integer NOT NULL,
  quest_mission_id integer NOT NULL,
  progress_value integer,
  is_clear boolean,
  latest_clear_datetime timestamp,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, quest_id, quest_mission_id)
);
CREATE INDEX i_user_quest_mission_user_id ON i_user_quest_mission(user_id);

CREATE TRIGGER i_user_quest_mission_update_version
AFTER UPDATE ON i_user_quest_mission FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_quest_mission SET latest_version = OLD.latest_version + 1
    WHERE i_user_quest_mission.rowid = NEW.rowid;
END;

CREATE TABLE i_user_quest_replay_flow_reward_group (
  user_id INTEGER NOT NULL,
  quest_replay_flow_reward_group_id integer NOT NULL,
  receive_datetime timestamp,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, quest_replay_flow_reward_group_id)
);
CREATE INDEX i_user_quest_replay_flow_reward_group_user_id ON i_user_quest_replay_flow_reward_group(user_id);

CREATE TRIGGER i_user_quest_replay_flow_reward_group_update_version
AFTER UPDATE ON i_user_quest_replay_flow_reward_group FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_quest_replay_flow_reward_group SET latest_version = OLD.latest_version + 1
    WHERE i_user_quest_replay_flow_reward_group.rowid = NEW.rowid;
END;

CREATE TABLE i_user_quest_scene_choice (
  user_id INTEGER NOT NULL,
  quest_scene_choice_grouping_id integer NOT NULL,
  quest_scene_choice_effect_id integer NOT NULL,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY (user_id, quest_scene_choice_grouping_id)
);
CREATE INDEX i_user_quest_scene_choice_user_id ON i_user_quest_scene_choice(user_id);

CREATE TRIGGER i_user_quest_scene_choice_update_version
AFTER UPDATE ON i_user_quest_scene_choice FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_quest_scene_choice SET latest_version = OLD.latest_version + 1
    WHERE i_user_quest_scene_choice.rowid = NEW.rowid;
END;

CREATE TABLE i_user_quest_scene_choice_history (
  user_id INTEGER NOT NULL,
  quest_scene_choice_effect_id integer NOT NULL,
  choice_datetime timestamp NOT NULL,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, quest_scene_choice_effect_id)
);
CREATE INDEX i_user_quest_scene_choice_history_user_id ON i_user_quest_scene_choice_history(user_id);

CREATE TRIGGER i_user_quest_scene_choice_history_update_version
AFTER UPDATE ON i_user_quest_scene_choice_history FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_quest_scene_choice_history SET latest_version = OLD.latest_version + 1
    WHERE i_user_quest_scene_choice_history.rowid = NEW.rowid;
END;

CREATE TABLE i_user_setting (
  user_id INTEGER NOT NULL PRIMARY KEY,
  is_notify_purchase_alert boolean NOT NULL DEFAULT 1,
  latest_version bigint NOT NULL DEFAULT 1
);

CREATE TRIGGER i_user_setting_update_version
AFTER UPDATE ON i_user_setting FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_setting SET latest_version = OLD.latest_version + 1
    WHERE i_user_setting.rowid = NEW.rowid;
END;

CREATE TABLE i_user_shop_item (
  user_id INTEGER NOT NULL,
  shop_item_id integer NOT NULL,
  bought_count integer,
  latest_bought_count_changed_datetime timestamp,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, shop_item_id)
);

CREATE INDEX i_user_shop_item_user_id ON i_user_shop_item(user_id);

CREATE TRIGGER i_user_shop_item_update_version
AFTER UPDATE ON i_user_shop_item FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_shop_item SET latest_version = OLD.latest_version + 1
    WHERE i_user_shop_item.rowid = NEW.rowid;
END;

CREATE TABLE i_user_shop_replaceable (
  user_id INTEGER NOT NULL PRIMARY KEY,
  lineup_update_count integer,
  latest_lineup_update_datetime timestamp,
  latest_version bigint NOT NULL DEFAULT 1
);

CREATE TRIGGER i_user_shop_replaceable_update_version
AFTER UPDATE ON i_user_shop_replaceable FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_shop_replaceable SET latest_version = OLD.latest_version + 1
    WHERE i_user_shop_replaceable.rowid = NEW.rowid;
END;

CREATE TABLE i_user_shop_replaceable_lineup (
  user_id INTEGER NOT NULL,
  slot_number integer NOT NULL,
  shop_item_id integer,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, slot_number)
);
CREATE INDEX i_user_shop_replaceable_lineup_user_id ON i_user_shop_replaceable_lineup(user_id);

CREATE TRIGGER i_user_shop_replaceable_lineup_update_version
AFTER UPDATE ON i_user_shop_replaceable_lineup FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_shop_replaceable_lineup SET latest_version = OLD.latest_version + 1
    WHERE i_user_shop_replaceable_lineup.rowid = NEW.rowid;
END;

CREATE TABLE i_user_side_story_quest (
  user_id INTEGER NOT NULL,
  side_story_quest_id integer NOT NULL,
  head_side_story_quest_scene_id integer,
  side_story_quest_state_type integer,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, side_story_quest_id)
);
CREATE INDEX i_user_side_story_quest_user_id ON i_user_side_story_quest(user_id);

CREATE TRIGGER i_user_side_story_quest_update_version
AFTER UPDATE ON i_user_side_story_quest FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_side_story_quest SET latest_version = OLD.latest_version + 1
    WHERE i_user_side_story_quest.rowid = NEW.rowid;
END;

CREATE TABLE i_user_side_story_quest_scene_progress_status (
  user_id INTEGER NOT NULL PRIMARY KEY,
  current_side_story_quest_id integer,
  current_side_story_quest_scene_id integer,
  latest_version bigint NOT NULL DEFAULT 1
);

CREATE TRIGGER i_user_side_story_quest_scene_progress_status_update_version
AFTER UPDATE ON i_user_side_story_quest_scene_progress_status FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_side_story_quest_scene_progress_status SET latest_version = OLD.latest_version + 1
    WHERE i_user_side_story_quest_scene_progress_status.rowid = NEW.rowid;
END;

CREATE TABLE i_user_status (
  user_id INTEGER NOT NULL PRIMARY KEY,
  level integer NOT NULL DEFAULT 1,
  exp integer NOT NULL DEFAULT 0,
  stamina_milli_value integer NOT NULL DEFAULT 50000,
  stamina_update_datetime timestamp NOT NULL DEFAULT 0,
  latest_version bigint NOT NULL DEFAULT 1
);

CREATE TRIGGER i_user_status_update_version
AFTER UPDATE ON i_user_status FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_status SET latest_version = OLD.latest_version + 1
    WHERE i_user_status.rowid = NEW.rowid;
END;

CREATE TRIGGER i_user_status_update_stamina_datetime
AFTER UPDATE OF stamina_milli_value ON i_user_status
BEGIN
  UPDATE i_user_status SET stamina_update_datetime = current_net_timestamp() WHERE rowid = NEW.rowid;
END;

CREATE TRIGGER i_user_status_initialize_stamina_datetime
AFTER INSERT ON i_user_status FOR EACH ROW
BEGIN
  UPDATE i_user_status SET stamina_update_datetime = current_net_timestamp() WHERE rowid = NEW.rowid;
END;

CREATE TABLE i_user_thought (
  user_id INTEGER NOT NULL,
  user_thought_uuid text NOT NULL,
  thought_id integer NOT NULL,
  acquisition_datetime timestamp NOT NULL,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, user_thought_uuid)
);
CREATE INDEX i_user_thought_user_id ON i_user_thought(user_id);

CREATE TRIGGER i_user_thought_update_version
AFTER UPDATE ON i_user_thought FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_thought SET latest_version = OLD.latest_version + 1
    WHERE i_user_thought.rowid = NEW.rowid;
END;

CREATE TABLE i_user_triple_deck (
  user_id INTEGER NOT NULL,
  deck_type integer NOT NULL,
  user_deck_number integer NOT NULL,
  name text,
  deck_number01 integer,
  deck_number02 integer,
  deck_number03 integer,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, deck_type, user_deck_number)
);
CREATE INDEX i_user_triple_deck_user_id ON i_user_triple_deck(user_id);

CREATE TRIGGER i_user_triple_deck_update_version
AFTER UPDATE ON i_user_triple_deck FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_triple_deck SET latest_version = OLD.latest_version + 1
    WHERE i_user_triple_deck.rowid = NEW.rowid;
END;

CREATE TABLE i_user_tutorial_progress (
  user_id INTEGER NOT NULL,
  tutorial_type integer NOT NULL,
  progress_phase integer,
  choice_id integer,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, tutorial_type)
);
CREATE INDEX i_user_tutorial_progress_user_id ON i_user_tutorial_progress(user_id);

CREATE TRIGGER i_user_tutorial_progress_update_version
AFTER UPDATE ON i_user_tutorial_progress FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_tutorial_progress SET latest_version = OLD.latest_version + 1
    WHERE i_user_tutorial_progress.rowid = NEW.rowid;
END;

CREATE TABLE i_user_weapon (
  user_id INTEGER NOT NULL,
  user_weapon_uuid text NOT NULL,
  weapon_id integer NOT NULL,
  level integer NOT NULL DEFAULT 1,
  exp integer NOT NULL DEFAULT 0,
  limit_break_count integer NOT NULL DEFAULT 0,
  is_protected boolean NOT NULL DEFAULT 0,
  acquisition_datetime timestamp NOT NULL,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, user_weapon_uuid)
);
CREATE INDEX i_user_weapon_user_id ON i_user_weapon(user_id);

CREATE TRIGGER i_user_weapon_update_version
AFTER UPDATE ON i_user_weapon FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_weapon SET latest_version = OLD.latest_version + 1
    WHERE i_user_weapon.rowid = NEW.rowid;
END;

CREATE TABLE i_user_weapon_ability (
  user_id INTEGER NOT NULL,
  user_weapon_uuid text NOT NULL,
  slot_number integer NOT NULL,
  level integer NOT NULL DEFAULT 1,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, user_weapon_uuid, slot_number)
);
CREATE INDEX i_user_weapon_ability_user_id ON i_user_weapon_ability(user_id);

CREATE TRIGGER i_user_weapon_ability_update_version
AFTER UPDATE ON i_user_weapon_ability FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_weapon_ability SET latest_version = OLD.latest_version + 1
    WHERE i_user_weapon_ability.rowid = NEW.rowid;
END;

CREATE TABLE i_user_weapon_awaken (
  user_id INTEGER NOT NULL,
  user_weapon_uuid text NOT NULL,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, user_weapon_uuid)
);
CREATE INDEX i_user_weapon_awaken_user_id ON i_user_weapon_awaken(user_id);

CREATE TRIGGER i_user_weapon_awaken_update_version
AFTER UPDATE ON i_user_weapon_awaken FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_weapon_awaken SET latest_version = OLD.latest_version + 1
    WHERE i_user_weapon_awaken.rowid = NEW.rowid;
END;

CREATE TABLE i_user_weapon_note (
  user_id INTEGER NOT NULL,
  weapon_id integer NOT NULL,
  max_level integer NOT NULL DEFAULT 1,
  max_limit_break_count integer NOT NULL DEFAULT 0,
  first_acquisition_datetime timestamp NOT NULL,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, weapon_id)
);
CREATE INDEX i_user_weapon_note_user_id ON i_user_weapon_note(user_id);

CREATE TRIGGER i_user_weapon_note_update_version
AFTER UPDATE ON i_user_weapon_note FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_weapon_note SET latest_version = OLD.latest_version + 1
    WHERE i_user_weapon_note.rowid = NEW.rowid;
END;

CREATE TABLE i_user_weapon_skill (
  user_id INTEGER NOT NULL,
  user_weapon_uuid text NOT NULL,
  slot_number integer NOT NULL,
  level integer NOT NULL DEFAULT 1,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, user_weapon_uuid, slot_number)
);

CREATE INDEX i_user_weapon_skill_user_id ON i_user_weapon_skill(user_id);

CREATE TRIGGER i_user_weapon_skill_update_version
AFTER UPDATE ON i_user_weapon_skill FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_weapon_skill SET latest_version = OLD.latest_version + 1
    WHERE i_user_weapon_skill.rowid = NEW.rowid;
END;

CREATE TABLE i_user_weapon_story (
  user_id INTEGER NOT NULL,
  weapon_id integer NOT NULL,
  released_max_story_index integer NOT NULL DEFAULT 0,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, weapon_id)
);

CREATE INDEX i_user_weapon_story_user_id ON i_user_weapon_story(user_id);

CREATE TRIGGER i_user_weapon_story_update_version
AFTER UPDATE ON i_user_weapon_story FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_weapon_story SET latest_version = OLD.latest_version + 1
    WHERE i_user_weapon_story.rowid = NEW.rowid;
END;

CREATE TABLE i_user_webview_panel_mission (
  user_id INTEGER NOT NULL,
  webview_panel_mission_page_id integer NOT NULL,
  reward_receive_datetime timestamp NOT NULL,
  latest_version bigint NOT NULL DEFAULT 1,
  PRIMARY KEY(user_id, webview_panel_mission_page_id)
);

CREATE INDEX i_user_webview_panel_mission_user_id ON i_user_webview_panel_mission(user_id);

CREATE TRIGGER i_user_webview_panel_mission_update_version
AFTER UPDATE ON i_user_webview_panel_mission FOR EACH ROW WHEN OLD.latest_version = NEW.latest_version
BEGIN
  UPDATE i_user_webview_panel_mission SET latest_version = OLD.latest_version + 1
    WHERE i_user_webview_panel_mission.rowid = NEW.rowid;
END;

CREATE TABLE internal_user_quest_last_start_attributes (
  user_id INTEGER NOT NULL,
  quest_id INTEGER NOT NULL,
  user_deck_number INTEGER NOT NULL,
  PRIMARY KEY(user_id, quest_id)
);
