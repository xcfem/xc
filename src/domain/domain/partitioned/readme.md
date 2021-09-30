# Partitioned domain

A partitioned domain is an aggregation of subdomains. All elements, nodes, loadcases are added to the PartitionedDomain. The components can be moved among subdomains (keeping in mind that subdomains are themselves subclasses of domain and therefore must obey the requirements for adding and removing elements specified in the interface for Domain) by invoking the remove..() and add...() methods on the subdomain.
