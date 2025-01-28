#ifndef FUNCTION_H
#define FUNCTION_H


void zk_update(void);


//四种开锁模式
u8 open_passward(u8 bs8116_key);
void open_fingerprint(void);
void open_card(void);



void main_page(u8 key);
void admin_page(u8 key);
void password_page(u8 key);
void mg200_page(u8 key);
void Enroll_user_page(u8 key);
void erase_user_one_page(u8 key);
void erase_user_all_page(u8 key);
void erase_user_match_page(u8 key);
void rfid_page(u8 key);
void picc_user_page(u8 key);
void erase_user_picc_page(u8 key);
void erase_picc_all_page(u8 key);
void erase_picc_match_page(u8 key);
void voice_light_page(u8 key);
void factory_reset_page(u8 key);


#endif