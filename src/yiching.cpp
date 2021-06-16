#include<gtkmm.h>
#include<fstream>
//#include<iostream>
#include<string>
using namespace std;

enum Symbol {
	 LAND=0, THUNDER=1, WATER=2, LAKE=3, MOUNTAIN=4, FIRE=5, WIND=6, SKY=7
};

struct Gwe
{
	char gwe_;//upper bits are first 0b100 = mountain
	string name_, gphrase_, gsang_, gdan_, hphrase_[6], hsang_[6];
	//괘명, 괘사, 상, 단, 효사, 상
} g[64];

const string chinese = "地雷水泽山火风天";

string remove_head(string s) {
	return s;
	string to_find = (s.find("，") != string::npos) ? "，" : "：";//wierd utf-8 string
	return s.substr(s.find(to_find) + 3);//not 1 byte, but 3 byte
}

string psstm(string command)
{//return system call output as string
	string s;
	char tmp[1000];
	FILE* f = popen(command.c_str(), "r");
	while(fgets(tmp, sizeof(tmp), f)) s += tmp;
	pclose(f);
	return s;
}

void parse_file() {
	string s;
	ifstream f{"주역.txt"};
	for(int i=0; i<64; i++) {
		do f >> s;
		while(!s.starts_with("◎"));
		auto pos = s.find("】　　");//this is not space. wierd utf-8 character
		s = s.substr(pos + 9);
		if(chinese.find(s.substr(0, 3), 0) == string::npos) 
			s = s.substr(6, 3) + s.substr(6, 3) + s.substr(0, 3);
		//cout << s << endl;
		char up = chinese.find(s.substr(0, 3), 0) / 3;
		char down = chinese.find(s.substr(3, 3), 0) / 3;
		g[i].name_ = s.substr(6);
		g[i].gwe_ = up << 3 | down;
		for(int j=0; j<7; j++) getline(f, s);
		g[i].gphrase_ = remove_head(s);
		getline(f, s); getline(f, s);
		g[i].gdan_ = remove_head(s);
		getline(f, s); getline(f, s);
		g[i].gsang_ = remove_head(s);
		for(int j=0; j<6; j++) {
			getline(f, s); getline(f, s);
			g[i].hphrase_[j] = remove_head(s);
			if(i) {
				getline(f, s); getline(f, s);
				g[i].hsang_[j] = remove_head(s);
			}
		}
		//cout << +g[i].gwe_ << g[i].name_ << g[i].gphrase_ << g[i].gsang_ << g[i].gdan_;
		//for(int j=0; j<6; j++) cout << g[i].hphrase_[j] << g[i].hsang_[j];
	}
}

class Win : public Gtk::Window
{
public:
	Win() {
		add(grid_);
		grid_.attach(bt_, 0, 0);
		grid_.attach(phrase_[0], 1, 0);
		grid_.attach(change_, 2, 0);
		grid_.attach(phrase_[1], 3, 0);
		grid_.attach(save_, 4, 0);
		for(int i=0; i<6; i++) grid_.attach(hyo_phrase_[0][5-i], 0, i+1);
		for(int i=0; i<6; i++) grid_.attach(hyo_[0][5-i], 1, i+1);
		for(int i=0; i<6; i++) grid_.attach(check_[5-i], 2, i+1);
		for(int i=0; i<6; i++) grid_.attach(hyo_[1][5-i], 3, i+1);
		for(int i=0; i<6; i++) grid_.attach(hyo_phrase_[1][5-i], 4, i+1);

		for(int i=0; i<6; i++) for(int j=0; j<2; j++) {
			pix_[i][j][0].set(yin_img_.get_pixbuf()->scale_simple(300, 70, Gdk::INTERP_BILINEAR));
			pix_[i][j][1].set(yang_img_.get_pixbuf()->scale_simple(300, 70, Gdk::INTERP_BILINEAR));
		}
		for(int i=0; i<6; i++) hyo_[0][i].set_image(pix_[i][0][0]);
		change_.set_tooltip_text("變爻");
		show_all_children();
		connect_event();
	}
protected:
	Gtk::Grid grid_;
	Gtk::Button bt_{"Done"}, save_{"Save"};
	Gtk::CheckButton check_[6];
	Gtk::Label  hyo_phrase_[2][6], phrase_[2], change_{"변효"};
	Gtk::Button hyo_[2][6];
	bool yinyang_[6] = {false, };
	Gtk::Image yin_img_{"yin.png"}, yang_img_{"yang.png"}, pix_[6][2][2];
private:
	char c_, d_, e_;
//	string yin_ = "<span size=\"xx-large\">\u268b</span>",
//				 yang_ = "<span size=\"xx-large\">\u268a</span>";
	void connect_event() {
		for(int i=0; i<6; i++) hyo_[0][i].signal_clicked().connect([this, i]() {
				yinyang_[i] = !yinyang_[i];
				hyo_[0][i].set_image(yinyang_[i] ? pix_[i][0][1] : pix_[i][0][0]);
		});
		bt_.signal_clicked().connect([this]() {
				char c = 0, d = 0;
				int changed_count = 0;
				for(int i=0; i<6; i++) c += (yinyang_[i] ? 1 : 0) << i;
				for(int i=0; i<6; i++) if(check_[i].get_active()) {
					changed_count++;
					d +=  1 << i;
				} 
				char e = c ^ d;
				c_ = c; d_ = d; e_ = e;
				for(int i=0; i<6; i++) hyo_[1][i].set_image(e & (1 << i) ? pix_[i][1][1] : pix_[i][1][0]);
				for(int i=0; i<64; i++) {
					if(g[i].gwe_ == c) {
						phrase_[0].set_markup(g[i].gphrase_);
						phrase_[0].set_tooltip_text(g[i].gdan_ + '\n' + g[i].gsang_);
						for(int j=0; j<6; j++) {
							hyo_phrase_[0][j].set_markup(g[i].hphrase_[j]);
							hyo_phrase_[0][j].set_tooltip_text(g[i].hsang_[j]);
						}
					}
					if(g[i].gwe_ == e) {
						phrase_[1].set_markup(g[i].gphrase_);
						phrase_[1].set_tooltip_text(g[i].gdan_ + '\n' + g[i].gsang_);
						for(int j=0; j<6; j++) {
							hyo_phrase_[1][j].set_markup(g[i].hphrase_[j]);
							hyo_phrase_[1][j].set_tooltip_text(g[i].hsang_[j]);
						}
					}
				}
				switch(changed_count) {
					case 0: bluefy(phrase_[0]); break;
					case 1: bluefy(hyo_phrase_[0][get_lower(d)]); break;
					case 2: bluefy(hyo_phrase_[0][get_upper(d)]); break;
					case 3: bluefy(phrase_[0]); bluefy(phrase_[1]); break;
					case 4: bluefy(hyo_phrase_[1][get_lower(flip(d))]); break;
					case 5: bluefy(hyo_phrase_[1][get_lower(flip(d))]); break;
					case 6: bluefy(phrase_[1]); break;
				}
		});

		save_.signal_clicked().connect([this]() {
				ofstream f{"save.txt", ios_base::app};
				f << psstm("date") << +c_ << ' ' << +d_ << ' ' << +e_ << endl;
		});
	}
	void bluefy(Gtk::Label &l) {
		l.set_markup("<span foreground=\"blue\">" + l.get_label() + "</span>");
	}
	int get_upper(char c) {
		for(int i=0; i<6; i++) if(0b100000 >> i & c) return 5-i;
	}
	int get_lower(char c) {
		for(int i=0; i<6; i++) if(1 << i & c) return i;
	}
	char flip(char c) {
		return ~c & 0b00111111;
	}
};

int main(int ac, char** av)
{
	parse_file();
	auto app = Gtk::Application::create(ac, av, "");
	Win win;
	return app->run(win);
}
