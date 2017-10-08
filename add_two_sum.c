int addTwoSum(ListNode* l1, ListNode* l2)
{
    ListNode* l3 = (ListNode*)malloc(sizeof(struct ListNode));
    ListNode* l3_head = l3;
    int n1 = l1->val;
    int n2 = l2->val;

    int c = (n1 + n2) / 10;
    int r = (n1 + n2) % 10;

    l3->val = r;

    auto node1 = l1->next;
    auto node2 = l2->next;

    while (node1 != NULL && node2 != NULL) {
        ListNode* n3 = (ListNode*)malloc(sizeof(struct ListNode));
        n1 = node1->val;
        n2 = node2->val;
        r = (c + n1 + n2) % 10;
        c = (c + n1 + n2) / 10;
        n3->val = r;
        l3->next = n3;
        l3 = l3->next;

        node1 = node1->next;
        node2 = node2->next;
    }

    while (node1 != NULL) {
        ListNode* n3 = (ListNode*)malloc(sizeof(struct ListNode));
        n1 = node1->val;
        r = (n1 + c) % 10;
        c = (n1 + c) / 10;
        n3->val = r;
        l3->next = n3;
        l3 = l3->next;

        node1 = node1->next;
    }

    while (node2 != NULL) {
        ListNode* n3 = (ListNode*)malloc(sizeof(struct ListNode));
        n2 = node2->val;
        r = (n2 + c) % 10;
        c = (n2 + c) / 10;
        n3->val = r;
        l3->next = n3;
        l3 = l3->next;

        node2 = node2->next;
    }

    return l3_head;
}
