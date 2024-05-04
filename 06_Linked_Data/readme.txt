Commands -

CREATE [repo_name] [linked_repo_name] [expected_status]
OPEN [repo_name] [linked_repo_name] [expected_status]
CLOSE [expected_status]
STORE_PARENT [parent_id] [expected_status]
STORE_CHILD [child_id] [expected_status]
NDX_SEARCH [parent_id] [expected_status]
NDX_DELETE [parent_id] [expected_status]
NON_NDX_SEARCH [non_ndx_key] [io_count]
ADD_LINK [parent_key] [child_key] [expected_status]
GET_LINK parent_id [expected_status] + [no_of_links] [link_1] [link_2] ... (if expected_status = 0)

To check, use the following commands
    gcc -o pds_tester IMT2022076_pds_tester.c IMT2022076_pds.c structs.c bst.c
    ./pds_tester lab5testcase.in or lab6testcase.in

For the interactive part, use
    gcc IMT2022076_pds.c IMT2022076_demo.c bst.c structs.c
    ./a.out