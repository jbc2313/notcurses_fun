#include <notcurses/notcurses.h>
#include <locale.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static struct ncselector_item item_list[] = {
	#define SITEM(s,l){s,l,}
		SITEM("OPT 1", "This is OPT 1"),
		SITEM("OPT 2", "This is OPT 2"),
		SITEM(NULL,NULL), 
	#undef SITEM
};

static void selector_func(struct notcurses *nc, struct ncselector *select, char* user_choice)
{
	static int item = 0;
	++item;
	if(select == NULL){
		notcurses_stop(nc);
		fprintf(stderr, "selector creation %d failed\n", item);
		exit(EXIT_FAILURE);
	};
	notcurses_render(nc);
	uint32_t keyinput;
	ncinput ncinput;
	const char *tmp;
	while((keyinput = notcurses_get_blocking(nc, &ncinput)) != (uint32_t)-1){
		if(!ncselector_offer_input(select, &ncinput)){
			if(ncinput.evtype == NCTYPE_RELEASE){
				continue;
			}
			switch(keyinput){
				case NCKEY_ENTER:
					tmp = ncselector_selected(select);
					strcpy(user_choice,tmp);
					ncselector_destroy(select, NULL); 
					return;
				case 'M':
				case 'J':
					if(ncinput_ctrl_p(&ncinput)){
						ncselector_destroy(select, NULL);
						return;
					};
			};
			if(keyinput == 'q')
				break;
		}
		notcurses_render(nc);
	}
	ncselector_destroy(select, NULL);
};

int main(void) 
{
	setlocale(LC_ALL, "");
	notcurses_options noopts = { };
	struct notcurses *nc = notcurses_init(&noopts, NULL);
	if(nc==NULL)
		return EXIT_FAILURE;
	notcurses_mice_enable(nc, NCMICE_BUTTON_EVENT);
	// one way to init options	
	// ncselector_options opts = {
	// 	.title = NULL,
	// 	.secondary = "THIS IS A SECONDARY MENU",
	// 	.items = item_list
	// };
	ncselector_options opts;
	memset(&opts, 0, sizeof(opts));
	opts.maxdisplay=4;
	opts.items = item_list;
	opts.title= "TITLE HERE";
	opts.secondary="SECONDARY TITLE HERE";
	opts.footer="Press 'q' to exit";
	opts.defidx=1;
	opts.boxchannels = NCCHANNELS_INITIALIZER(0x20, 0xe0, 0x40, 0x20, 0x20, 0x20);
        opts.opchannels = NCCHANNELS_INITIALIZER(0xe0, 0x80, 0x40, 0, 0, 0);
        opts.descchannels = NCCHANNELS_INITIALIZER(0x80, 0xe0, 0x40, 0, 0, 0);
        opts.footchannels = NCCHANNELS_INITIALIZER(0xe0, 0, 0x40, 0x20, 0, 0);
        opts.titlechannels = NCCHANNELS_INITIALIZER(0xff, 0xff, 0x80, 0, 0, 0x20);
        uint64_t bgchannels = NCCHANNELS_INITIALIZER(0, 0x20, 0, 0, 0x20, 0);
        ncchannels_set_fg_alpha(&bgchannels, NCALPHA_BLEND);
        ncchannels_set_bg_alpha(&bgchannels, NCALPHA_BLEND);
        struct ncplane* n = notcurses_stdplane(nc);

/*
 *  FOR IMAGES
	if(notcurses_canopen_images(nc)){
    struct ncvisual* ncv = ncvisual_from_file("../data/changes.jpg");
    if(!ncv){
      goto err;
    }
    struct ncvisual_options vopts = {
      .scaling = NCSCALE_STRETCH,
      .n = n,
    };
    if(ncvisual_blit(nc, ncv, &vopts) == NULL){
      goto err;
    }
    ncvisual_destroy(ncv);
  }
*/

	ncplane_set_fg_rgb(n, 0x40f040);
	ncplane_putstr_aligned(n, 0, NCALIGN_RIGHT, "selector widget demo");
	struct ncplane_options nplaneopts = {
		.y = 3,
		.x = 0,
		.rows = 10, //was 1
		.cols = 10, //was 1
		.userptr = NULL,
		.name = NULL,
		.resizecb = NULL,
		.flags = 0,
	};

	struct ncplane_options nplanechoiceopts = {
		.y = 15,
		.x = 15,
		.rows = 10, //was 1
		.cols = 20, //was 1
		.userptr = NULL,
		.name = NULL,
		.resizecb = NULL,
		.flags = 0,
	};

	struct ncplane *plane1 = ncplane_create(n, &nplaneopts);
	ncplane_set_base(plane1, "", 0, bgchannels);
	struct ncselector *list = ncselector_create(plane1, &opts);
	// create variable to get users choice from selector_func
	const char *choice;
	char *tmp1 = malloc(sizeof(char)*10);
	selector_func(nc, list, tmp1);
	// output user choice to new ncplane and wait 10 sec and die
	struct ncplane *plane2 = ncplane_create(n, &nplanechoiceopts);
	ncplane_set_base(plane2, "", 0, bgchannels);
	//ncplane_putchar_yx(plane2, 2, 2, 'F');
	//const char *x = "Welcome to the show";
	choice = tmp1;
	ncplane_putstr(plane2, choice);	
	notcurses_render(nc);	
	sleep(2);



	if(notcurses_stop(nc))
		return EXIT_FAILURE;
	return EXIT_SUCCESS;

	err: 
		notcurses_stop(nc);
		return EXIT_FAILURE;
	

	//memset(&noopts, 0 , sizeof(noopts));
	//noopts.flags = NCOPTION_NO_ALTERNATE_SCREEN;
	// int dimy, dimx;
	// notcurses_render(nc);
	// struct ncselector *list = ncselector_create(plane1, &opts);
	// notcurses_render(nc);
	// notcurses_refresh(nc,0,0);
	// ncplane_putchar(plane1, 'x');
	// const ncinput *iput;
	// ncselector_offer_input(list, iput);
	// const char *selected = ncselector_selected(list);
	// 
	// while(selected != item_list[0].option){
	// 	notcurses_render(nc);
	// };
	//
	//
	// return notcurses_stop(nc);

	//return (notcurses_stop(nc) || notcurses_render(nc)) ? EXIT_FAILURE : EXIT_SUCCESS;
};
