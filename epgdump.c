#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <iconv.h>
#include <time.h>

#include "ts.h"
#include "sdt.h"
#include "eit.h"
#include "ts_ctl.h"

typedef		struct	_ContentTYPE{
	char	*japanese ;
	char	*english ;
}CONTENT_TYPE;

#define		CAT_COUNT		16
static  CONTENT_TYPE	ContentCatList[CAT_COUNT] = {
	{ "ニュース・報道", "news" },
	{ "スポーツ", "sports" },
	{ "情報", "information" },
	{ "ドラマ", "drama" },
	{ "音楽", "music" },
	{ "バラエティ", "variety" },
	{ "映画", "cinema" },
	{ "アニメ・特撮", "anime" },
	{ "ドキュメンタリー・教養", "documentary" },
	{ "演劇", "stage" },
	{ "趣味・実用", "hobby" },
	{ "福祉", "etc" },			//福祉
	{ "予備", "etc" }, //予備
	{ "予備", "etc" }, //予備
	{ "予備", "etc" }, //予備
	{ "その他", "etc" } //その他
};
typedef struct _TAG_STATION
{
	char	*name;
	char	*ontv;
	int		tsId;		// OriginalNetworkID
	int		onId;		// TransportStreamID
	int		svId;		// ServiceID
} STATION;

static STATION bsSta[] = {
	{ "NHK BS1", "3001.ontvjapan.com", 16625, 4, 101},
	{ "NHK BS2", "3002.ontvjapan.com", 16625, 4, 102},//廃止 2011/3/31 
	{ "NHK BSプレミアム1", "3003.ontvjapan.com", 16433, 4, 103},
//	{ "NHK BSプレミアム2", "3004.ontvjapan.com", 16433, 4, 104},
	{ "BS日テレ", "3004.ontvjapan.com", 16592, 4, 141},
	{ "BS朝日", "3005.ontvjapan.com", 16400, 4, 151},
	{ "BS-TBS", "3006.ontvjapan.com", 16401, 4, 161},
	{ "ＢＳテレ東", "3007.ontvjapan.com", 16402, 4, 171},
	{ "BSフジ", "3008.ontvjapan.com", 16593, 4, 181},
//	{ "WOWOW", "3009.ontvjapan.com", 16432, 4, 191},
//	{ "WOWOW2", "3010.ontvjapan.com", 16432, 4, 192},
//	{ "WOWOW3", "3011.ontvjapan.com", 16432, 4, 193},
	{ "WOWOWプライム", "3009.ontvjapan.com", 16432, 4, 191},
	{ "WOWOWライブ", "4192.epgdata.ontvjapan", 17488, 4, 192},
	{ "WOWOWシネマ", "4193.epgdata.ontvjapan", 17489, 4, 193},
//	{ "スター・チャンネル", "3012.ontvjapan.com", 16529, 4, 200},
	{ "スター・チャンネル1", "3012.ontvjapan.com", 16529, 4, 200},
	{ "スター・チャンネル2", "4201.epgdata.ontvjapan", 16626, 4, 201},
	{ "スター・チャンネル3", "4202.epgdata.ontvjapan", 16626, 4, 202},
	{ "BS11", "3013.ontvjapan.com", 16528, 4, 211},
	{ "TwellV", "3014.ontvjapan.com", 16530, 4, 222},
	{ "放送大学1", "4231.epgdata.ontvjapan", 18098, 4, 231},
	{ "放送大学2", "4232.epgdata.ontvjapan", 18098, 4, 232},
	{ "放送大学3", "4233.epgdata.ontvjapan", 18098, 4, 233},
	{ "グリーンチャンネル", "4234.epgdata.ontvjapan", 18258, 4, 234},
	{ "BSアニマックス", "1047.ontvjapan.com", 18130, 4, 236},
//	{ "FOX bs238", "4238.epgdata.ontvjapan", 18096, 4, 238},
	{ "BSスカパー！", "4241.epgdata.ontvjapan", 18097, 4, 241},
	{ "J SPORTS 1", "4242.epgdata.ontvjapan", 18225, 4, 242},
	{ "J SPORTS 2", "4243.epgdata.ontvjapan", 18226, 4, 243},
	{ "J SPORTS 3", "4244.epgdata.ontvjapan", 18257, 4, 244},
	{ "J SPORTS 4", "4245.epgdata.ontvjapan", 18258, 4, 245},
	{ "BS釣りビジョン", "4251.epgdata.ontvjapan", 18099, 4, 251},
	{ "ＷＯＷＯＷプラス", "4252.epgdata.ontvjapan", 18256, 4, 252},
	{ "日本映画専門チャンネル", "4255.epgdata.ontvjapan", 18257, 4, 255},
	{ "ディズニー・チャンネル", "1090.ontvjapan.com", 18288, 4, 256},
	{ "BS松竹東急", "4260.epgdata.ontvjapan", 18803,4, 260},
	{ "BSJapanext", "4263.epgdata.ontvjapan", 18802, 4, 263},
	{ "BSよしもと", "4265.epgdata.ontvjapan", 18801, 4,265},
//	{ "NHK総合テレビジョン（東京）", "4291.epgdata.ontvjapan", 17168, 4, 291},
//	{ "NHK教育テレビジョン（東京）", "4292.epgdata.ontvjapan", 17168, 4, 292},
//	{ "日本テレビ", "4294.epgdata.ontvjapan", 17169, 4, 294},
//	{ "テレビ朝日", "4295.epgdata.ontvjapan", 17169, 4, 295},
//	{ "TBSテレビ", "4296.epgdata.ontvjapan", 17169, 4, 296},
//	{ "テレビ東京", "4297.epgdata.ontvjapan", 17169, 4, 297},
//	{ "フジテレビ", "4298.epgdata.ontvjapan", 17168, 4, 298},
	{ "放送大学ラジオ", "4531.epgdata.ontvjapan", 18098, 4, 531},
	{ "ご案内チャンネル", "4791.ontvjapan.com", 16432, 4, 791},
//	{ "WNI", "4910.ontvjapan.com", 16626, 4, 910},
};

static int bsStaCount = sizeof(bsSta) / sizeof (STATION);



static STATION csSta[] = {
//	{ "スターｃｈプラス", "1002.ontvjapan.com", 24608, 6, 237},//廃止
//	{ "日本映画専門ｃｈＨＤ", "1086.ontvjapan.com", 24608, 6, 239},//BS変更
	{ "フジテレビＮＥＸＴ", "309ch.epgdata.ontvjapan", 24608, 6, 309},//306→309にチャンネル変更
	{ "ショップチャンネル", "1059.ontvjapan.com", 24704, 6, 55},
	{ "ザ・シネマ", "1217.ontvjapan.com", 24736, 6, 227},//228→227
	{ "スカチャン0 HD", "800ch.epgdata.ontvjapan", 24736, 6, 800},
	{ "スカチャン1 HD", "801ch.epgdata.ontvjapan", 24736, 6, 801},
	{ "スカチャン2", "802ch.epgdata.ontvjapan", 24736, 6, 802},
	{ "ｅ２プロモ", "100ch.epgdata.ontvjapan", 28736, 7, 100},
//	{ "インターローカルＴＶ", "194ch.epgdata.ontvjapan", 28736, 7, 194},//廃止 2010/9/16/
//	{ "Ｊスポーツ　ＥＳＰＮ", "1025.ontvjapan.com", 28736, 7, 256},//BSに変更
	{ "ＦＯＸ", "1016.ontvjapan.com", 28736, 7, 312},
	{ "FOXプラス", "315ch.epgdata.ontvjapan", 28736, 7, 315},
	{ "スペースシャワーＴＶ", "1018.ontvjapan.com", 28736, 7, 322},
	{ "カートゥーン　ネット", "1046.ontvjapan.com", 28736, 7, 331},
	{ "ディズニーＸＤ", "1213.ontvjapan.com", 28736, 7, 334},//トゥーン・ディズニー →
	{ "東映チャンネル", "1010.ontvjapan.com", 28768, 7, 221},
	{ "衛星劇場", "1005.ontvjapan.com", 28768, 7, 222},
	{ "チャンネルＮＥＣＯ", "1008.ontvjapan.com", 28768, 7, 223},
//	{ "洋画★シネフィル", "1009.ontvjapan.com", 28768, 7, 224},//BS変更
//	{ "スター・クラシック", "1003.ontvjapan.com", 28768, 7, 238},//BS変更
	{ "時代劇専門チャンネル", "1133.ontvjapan.com", 28768, 7, 292},
	{ "スーパードラマ", "1006.ontvjapan.com", 28768, 7, 310},
	{ "ＡＸＮ", "1014.ontvjapan.com", 28768, 7, 311},
	{ "ナショジオチャンネル", "1204.ontvjapan.com", 28768, 7, 343},
//	{ "ワンテンポータル", "110ch.epgdata.ontvjapan", 28864, 7, 110},//2011年11月30日廃止
//	{ "ゴルフチャンネル", "1028.ontvjapan.com", 28864, 7, 260},//2012年3月31日廃止
	{ "テレ朝チャンネルHD", "1092.ontvjapan.com", 28864, 6, 298},//2012年7月1日 CS1/Ch.298に変更
	{ "ＭＴＶ", "1019.ontvjapan.com", 28864, 7, 323},
	{ "ミュージック・エア", "1024.ontvjapan.com", 28864, 7, 324},
	{ "朝日ニュースターHD", "1067.ontvjapan.com", 28864, 6, 299},//2012年7月1日 CS1/Ch.299に変更
	{ "ＢＢＣワールド", "1070.ontvjapan.com", 28864, 7, 353},
	{ "ＣＮＮｊ", "1069.ontvjapan.com", 28864, 7, 354},
//	{ "ジャスト・アイ", "361ch.epgdata.ontvjapan", 28864, 7, 361},// 廃止 2011/8/31
	{ "ホームドラマチャンネル", "294ch.epgdata.ontvjapan", 28736, 7, 294}, 
//	{ "Ｊスポーツ　１", "1041.ontvjapan.com", 28896, 7, 251},// 2011/10/1 BSへ変更
//	{ "Ｊスポーツ　２", "1042.ontvjapan.com", 28896, 7, 252},// 2011/10/1 BSへ変更
//	{ "ＪスポーツＰｌｕｓＨ", "1043.ontvjapan.com", 28896, 7, 253},//BS変更
	{ "ＧＡＯＲＡ", "1026.ontvjapan.com", 28896, 7, 254},
	{ "スカイ・A sports＋", "1040.ontvjapan.com", 28896, 7, 250},//2012年1月24日 Ch.255からCh.250に変更
//	{ "宝塚プロモチャンネル", "101ch.epgdata.ontvjapan", 28928, 7, 101},
	{ "TAKARAZUKA SKY STAGE", "1207.ontvjapan.com", 28928, 7, 290},
	{ "チャンネル銀河", "305ch.epgdata.ontvjapan", 28928, 7, 305},
	{ "ＡＴ-Ｘ", "1201.ontvjapan.com", 28928, 7, 333},
	{ "ヒストリーチャンネル", "1050.ontvjapan.com", 28928, 7, 342},
//	{ "スカチャン８０３", "803ch.epgdata.ontvjapan", 28928, 7, 803},//スカチャン3に変更
//	{ "スカチャン８０４", "804ch.epgdata.ontvjapan", 28928, 7, 804},
	{ "スカチャン3", "805ch.epgdata.ontvjapan", 28928, 7, 805},
	{ "ムービープラスＨＤ", "1007.ontvjapan.com", 28960, 7, 240},
	{ "ゴルフネットワーク", "1027.ontvjapan.com", 28960, 7, 262},
	{ "ＬａＬａ　ＨＤ", "1074.ontvjapan.com", 28960, 7, 314},
	{ "フジテレビＯＮＥ", "1073.ontvjapan.com", 28992, 7, 307},//フジテレビ739→
	{ "フジテレビＴＷＯ", "1072.ontvjapan.com", 28992, 7, 308},//フジテレビ721→
//	{ "アニマックス", "1047.ontvjapan.com", 28992, 7, 332},//BSアニマックスに移動
	{ "ディスカバリー", "1062.ontvjapan.com", 28992, 7, 340},
	{ "アニマルプラネット", "1193.ontvjapan.com", 28992, 7, 341},
//	{ "Ｃ-ＴＢＳウエルカム", "160ch.epgdata.ontvjapan", 29024, 7, 160},//2012年3月31日廃止
	{ "ＱＶＣ", "1120.ontvjapan.com", 29024, 7, 161},
//	{ "プライム３６５．ＴＶ", "185ch.epgdata.ontvjapan", 29024, 7, 185},//2012年3月31日 廃止
	{ "ファミリー劇場", "1015.ontvjapan.com", 29024, 7, 293},
	{ "TBSチャンネルHD", "3201.ontvjapan.com", 29024, 6, 296},//2012年7月1日 CS1/Ch.296に変更
	
//	{ "ディズニーチャンネル", "1090.ontvjapan.com", 29024, 7, 304},//BSへ変更
	{ "MUSIC ON! TV", "1022.ontvjapan.com", 29024, 7, 325},
	{ "キッズステーションHD", "1045.ontvjapan.com", 29024, 7, 330},//HDに
	{ "ＴＢＳニュースバード", "1076.ontvjapan.com", 29024, 7, 351},
//	{ "ＣＳ日本番組ガイド", "147ch.epgdata.ontvjapan", 29056, 7, 147},//廃止 2010/2/28
	{ "日テレＧ＋ＨＤ", "1068.ontvjapan.com", 29056, 7, 257},//HD化
//	{ "fashion TV", "5004.ontvjapan.com", 29056, 7, 291},//廃止 2009/3/31
	{ "日テレプラス", "300ch.epgdata.ontvjapan", 29056, 7, 300},
//	{ "エコミュージックＴＶ", "1023.ontvjapan.com", 29056, 7, 320},//廃止	2009/3/31
//	{ "Music Japan TV", "1208.ontvjapan.com", 29056, 7, 321},//廃止 2012/3/31
	{ "スペースシャワーＴＶ プラス", "321ch.epgdata.ontvjapan", 29056, 7, 321},
	{ "日テレＮＥＷＳ２４", "2002.ontvjapan.com", 29056, 7, 350},
	{ "旅チャンネル", "1052.ontvjapan.com", 29056, 7, 362},
};

static int csStaCount = sizeof(csSta) / sizeof (STATION);
SVT_CONTROL	*svttop = NULL;
#define		SECCOUNT	4
char	title[1024];
char	subtitle[1024];
char	Category[1024];
char	ServiceName[1024];

/* prototype */
extern int strrep(char *buf, char *mae, char *ato);

void	xmlspecialchars(char *str)
{
	strrep(str, "&", "&amp;");
	strrep(str, "'", "&apos;");
	strrep(str, "\"", "&quot;");
	strrep(str, "<", "&lt;");
	strrep(str, ">", "&gt;");
}



void	GetSDT(FILE *infile, SVT_CONTROL *svttop, SECcache *secs, int count)
{
	SECcache  *bsecs;

	while((bsecs = readTS(infile, secs, count)) != NULL) {
		/* SDT */
		if((bsecs->pid & 0xFF) == 0x11) {
			dumpSDT(bsecs->buf, svttop);
		}
	}
}
void	GetEIT(FILE *infile, FILE *outfile, STATION *psta, SECcache *secs, int count)
{
	SECcache  *bsecs;
	EIT_CONTROL	*eitcur ;
	EIT_CONTROL	*eitnext ;
	EIT_CONTROL	*eittop = NULL;
	time_t	l_time ;
	time_t	end_time ;
	struct	tm	tl ;
	struct	tm	*endtl ;
	char	cendtime[32];
	char	cstarttime[32];

	eittop = calloc(1, sizeof(EIT_CONTROL));
	eitcur = eittop ;
	fseek(infile, 0, SEEK_SET);
	while((bsecs = readTS(infile, secs, SECCOUNT)) != NULL) {
		/* EIT */
		if((bsecs->pid & 0xFF) == 0x12) {
			dumpEIT(bsecs->buf, psta->svId, psta->onId, psta->tsId, eittop);
		}else if((bsecs->pid & 0xFF) == 0x26) {
			dumpEIT(bsecs->buf, psta->svId, psta->onId, psta->tsId, eittop);
		}else if((bsecs->pid & 0xFF) == 0x27) {
			dumpEIT(bsecs->buf, psta->svId, psta->onId, psta->tsId, eittop);
		}
	}
	eitcur = eittop ;
	while(eitcur != NULL){
		if(!eitcur->servid){
			eitcur = eitcur->next ;
			continue ;
		}
		if(eitcur->content_type > CAT_COUNT){
			eitcur->content_type = CAT_COUNT -1 ;
		}
		memset(title, '\0', sizeof(title));
		strcpy(title, eitcur->title);
		xmlspecialchars(title);

		memset(subtitle, '\0', sizeof(subtitle));
		strcpy(subtitle, eitcur->subtitle);
		xmlspecialchars(subtitle);

		memset(Category, '\0', sizeof(Category));
		strcpy(Category, ContentCatList[eitcur->content_type].japanese);
		xmlspecialchars(Category);

		tl.tm_sec = eitcur->ss ;
		tl.tm_min = eitcur->hm ;
		tl.tm_hour = eitcur->hh ;
		tl.tm_mday = eitcur->dd ;
		tl.tm_mon = (eitcur->mm - 1);
		tl.tm_year = (eitcur->yy - 1900);
		tl.tm_wday = 0;
		tl.tm_isdst = 0;
		tl.tm_yday = 0;
		l_time = mktime(&tl);
		if((eitcur->ehh == 0) && (eitcur->emm == 0) && (eitcur->ess == 0)){
			(void)time(&l_time);
			end_time = l_time + (60 * 5);		// ５分後に設定
		endtl = localtime(&end_time);
		}else{
			end_time = l_time + eitcur->ehh * 3600 + eitcur->emm * 60 + eitcur->ess;
			endtl = localtime(&end_time);
		}
		memset(cendtime, '\0', sizeof(cendtime));
		memset(cstarttime, '\0', sizeof(cstarttime));
		strftime(cendtime, (sizeof(cendtime) - 1), "%Y%m%d%H%M%S", endtl);
		strftime(cstarttime, (sizeof(cstarttime) - 1), "%Y%m%d%H%M%S", &tl);
#if 1
		fprintf(outfile, "  <programme start=\"%s +0900\" stop=\"%s +0900\" channel=\"%s\">\n",	
				cstarttime, cendtime, psta->ontv);
		fprintf(outfile, "    <title lang=\"ja_JP\">%s</title>\n", title);
		fprintf(outfile, "    <desc lang=\"ja_JP\">%s</desc>\n", subtitle);
		fprintf(outfile, "    <category lang=\"ja_JP\">%s</category>\n", Category);
		fprintf(outfile, "    <category lang=\"en\">%s</category>\n", ContentCatList[eitcur->content_type].english);
		fprintf(outfile, "  </programme>\n");
#else
		fprintf(outfile, "(%x:%x:%x)%s,%s,%s,%s,%s,%s\n",
					eitcur->servid, eitcur->table_id, eitcur->event_id,
					cstarttime, cendtime,
					title, subtitle,
					Category,
					ContentCatList[eitcur->content_type].english);
#endif
#if 0
		fprintf(outfile, "(%x:%x)%04d/%02d/%02d,%02d:%02d:%02d,%02d:%02d:%02d,%s,%s,%s,%s\n",
					eitcur->table_id, eitcur->event_id,
					eitcur->yy, eitcur->mm, eitcur->dd,
					eitcur->hh, eitcur->hm, eitcur->ss,
					eitcur->ehh, eitcur->emm, eitcur->ess,
					eitcur->title, eitcur->subtitle,
					ContentCatList[eitcur->content_type].japanese,
					ContentCatList[eitcur->content_type].english);
#endif
		eitnext = eitcur->next ;
		free(eitcur->title);
		free(eitcur->subtitle);
		free(eitcur);
		eitcur = eitnext ;
	}
	free(eittop);
	eittop = NULL;
}
int main(int argc, char *argv[])
{

	FILE *infile = stdin;
	FILE *outfile = stdout;
	char	*arg_onTV ;
	int		staCount ;
	char *file;
	int   inclose = 0;
	int   outclose = 0;
	SVT_CONTROL	*svtcur ;
	SVT_CONTROL	*svtsave ;
	SECcache   secs[SECCOUNT];
	int		lp ;
	STATION	*pStas ;
	int		act ;

	/* 興味のあるpidを指定 */
	memset(secs, 0,  sizeof(SECcache) * SECCOUNT);
	secs[0].pid = 0x11;
	secs[1].pid = 0x12;
	secs[2].pid = 0x26;
	secs[3].pid = 0x27;

	if(argc == 4){
		arg_onTV = argv[1];
		file = argv[2];
		if(strcmp(file, "-")) {
			infile = fopen(file, "r");
			inclose = 1;
		}
		if(strcmp(argv[3], "-")) {
			outfile = fopen(argv[3], "w+");
			outclose = 1;
		}
	}else{
		fprintf(stdout, "Usage : %s {/BS|/CS} <tsFile> <outfile>\n", argv[0]);
		fprintf(stdout, "Usage : %s <ontvcode> <tsFile> <outfile>\n", argv[0]);
		fprintf(stdout, "\n");
		fprintf(stdout, "  ontvcode   Channel identifier (ex. ****.ontvjapan.com)\n");
		fprintf(stdout, "  /BS        BS mode\n");
		fprintf(stdout, "               This mode reads the data of all BS TV stations\n");
		fprintf(stdout, "               from one TS data.\n");
		fprintf(stdout, "  /CS        CS mode\n");
		fprintf(stdout, "               This mode reads the data of two or more CS TV stations\n");
		fprintf(stdout, "               from one TS data.\n");
		return 0;
	}

	if(strcmp(arg_onTV, "/BS") == 0){
		pStas = bsSta;
		staCount = bsStaCount;
		act = 0 ;
	}else if(strcmp(arg_onTV, "/CS") == 0){
		pStas = csSta;
		staCount = csStaCount;
		act = 0 ;
	}else{
		act = 1 ;
		svttop = calloc(1, sizeof(SVT_CONTROL));
		GetSDT(infile, svttop, secs, SECCOUNT);
		svtcur = svttop->next ;	//先頭
		if(svtcur == NULL){
			free(svttop);
			return 1;
		}

		pStas = calloc(1, sizeof(STATION));
		pStas->tsId = svtcur->transport_stream_id ;
		pStas->onId = svtcur->original_network_id ;
		pStas->svId = svtcur->event_id ;
		pStas->ontv = arg_onTV ;
		pStas->name = svtcur->servicename ;
		staCount = 1;
	}

	fprintf(outfile, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	fprintf(outfile, "<!DOCTYPE tv SYSTEM \"xmltv.dtd\">\n\n");
	fprintf(outfile, "<tv generator-info-name=\"tsEPG2xml\" generator-info-url=\"http://localhost/\">\n");

	for(lp = 0 ; lp < staCount ; lp++){
		memset(ServiceName, '\0', sizeof(ServiceName));
		strcpy(ServiceName, pStas[lp].name);
		xmlspecialchars(ServiceName);

		fprintf(outfile, "  <channel id=\"%s\">\n", pStas[lp].ontv);
		fprintf(outfile, "    <display-name lang=\"ja_JP\">%s</display-name>\n", ServiceName);
		fprintf(outfile, "  </channel>\n");
	}
	for(lp = 0 ; lp < staCount ; lp++){
		GetEIT(infile, outfile, &pStas[lp], secs, SECCOUNT);
	}
	fprintf(outfile, "</tv>\n");
	if(inclose) {
		fclose(infile);
	}

	if(outclose) {
		fclose(outfile);
	}
	if(act){
		free(pStas);
		svtcur = svttop ;	//先頭
		while(svtcur != NULL){
			svtsave = svtcur->next ;
			free(svtcur);
			svtcur = svtsave ;
		}
	}

	return 0;
}
