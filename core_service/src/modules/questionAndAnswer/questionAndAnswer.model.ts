export interface IQuestionAndAnswer {
  questionId?: number;
  domainId: number;
  uuid?: string;
  chatFlowId?: string;
  questionName?: string;
  question?: string;
  answer?: string;
  sent?: number;
  helpfull?: number;
  notHelpfull?: number;
  mapFlow?: string;
  language?: string[];
  intent?: string[];
  keywords?: string[];
  status?: number;
  version?: string;
  createdAt?: string;
  updatedAt?: string;
  limit?: number;
  offset?: number;
  searchByValue?: number;
  embedding?: any;
  websiteId?: string;
  discription?: string;
  baseName?: string;
  websiteUrl?: string;
  btnIntent?: number;
}

export interface IWebCrawler extends IQuestionAndAnswer {
  domainId: number;
  domain: string;
  attributes?: string[];
  htmlTags?: string[];
  name?: string;
  crawlDepth?: number;
  simultaneous?: number;
  maxCrawled?: number;
  index_url: string;
  ext: string;
}
