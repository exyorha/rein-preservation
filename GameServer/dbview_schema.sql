CREATE TEMPORARY VIEW dbview_character AS SELECT character_id AS id, name_character_text_id FROM m_character ORDER BY sort_order;
CREATE TEMPORARY VIEW dbview_costume AS SELECT costume_id AS id, character_id FROM m_costume;
