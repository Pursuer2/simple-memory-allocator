


struct init_param{
	void *mem_start;
	void *mem_end;
};

#if _DEBUG == 1
extern void print_mem_stat();
#endif

extern int mod_memory_flat_init(struct init_param *param);
