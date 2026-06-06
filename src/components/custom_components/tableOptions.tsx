
export const expandRowTable: any = {
    showExpandColumn: true,
    expandColumnPosition: "right",
    onlyOneExpanding: true,
    expandColumnRenderer: ({ expanded }:any) => {
      if (expanded) {
        return (
          <svg
            xmlns="http://www.w3.org/2000/svg"
            width="12"
            height="6"
            viewBox="0 0 12 6"
            fill="none"
            style={{ transform: "rotate(180deg)" }}
          >
            <path
              d="M6.00009 5.90231C5.39759 5.90231 4.83209 5.66791 4.40679 5.24121L0.553286 1.37691C0.260786 1.08391 0.261786 0.609307 0.554786 0.316407C0.848686 0.024407 1.32379 0.0244068 1.61579 0.318407L5.46879 4.18271C5.75249 4.46591 6.24809 4.46591 6.53079 4.18271L10.3843 0.318407C10.6763 0.0245068 11.1514 0.024507 11.4453 0.316407C11.7383 0.609407 11.7392 1.08401 11.4468 1.37691L7.59329 5.24121C7.16799 5.66801 6.60219 5.90231 6.00009 5.90231Z"
              fill="#757676"
            />
          </svg>
        );
      }
      return (
        <svg
          xmlns="http://www.w3.org/2000/svg"
          width="12"
          height="6"
          viewBox="0 0 12 6"
          fill="none"
        >
          <path
            d="M6.00009 5.90231C5.39759 5.90231 4.83209 5.66791 4.40679 5.24121L0.553286 1.37691C0.260786 1.08391 0.261786 0.609307 0.554786 0.316407C0.848686 0.024407 1.32379 0.0244068 1.61579 0.318407L5.46879 4.18271C5.75249 4.46591 6.24809 4.46591 6.53079 4.18271L10.3843 0.318407C10.6763 0.0245068 11.1514 0.024507 11.4453 0.316407C11.7383 0.609407 11.7392 1.08401 11.4468 1.37691L7.59329 5.24121C7.16799 5.66801 6.60219 5.90231 6.00009 5.90231Z"
            fill="#757676"
          />
        </svg>
      );
    },
};

export const customTotal = (from: number, to: number, size: number) => (
  <span className="react-bootstrap-table-pagination-total">
      {from} to {to} of {size} records
  </span>
);

export const tableOptions = {
  paginationSize: 3,
  pageStartIndex: 1,
  sizePerPage: 10,
  hidePageListOnlyOnePage: true,
  alwaysShowAllBtns: true,
  withFirstAndLast: false,
  prePageText: (
      <svg
          width="6"
          height="12"
          viewBox="0 0 6 12"
          fill="none"
          xmlns="http://www.w3.org/2000/svg"
      >
          <path
              d="M5.15283 11.6655C4.96143 11.6655 4.76953 11.5923 4.62303 11.4467L0.758329 7.59322C0.332529 7.16842 0.0977298 6.60302 0.0972298 6.00042C0.0967298 5.39782 0.331629 4.83242 0.758329 4.40572L4.62353 0.553223C4.91653 0.261223 5.39163 0.261223 5.68403 0.555223C5.97653 0.848223 5.97553 1.32282 5.68203 1.61572L1.81733 5.46822C1.67523 5.60982 1.59713 5.79932 1.59713 6.00042C1.59713 6.20062 1.67523 6.38912 1.81733 6.53072L5.68253 10.3842C5.97553 10.6772 5.97643 11.1518 5.68403 11.4447C5.53753 11.5922 5.34513 11.6655 5.15283 11.6655Z"
              fill="#757676"
          />
      </svg>
  ),
  nextPageText: (
      <svg
          xmlns="http://www.w3.org/2000/svg"
          width="6"
          height="12"
          viewBox="0 0 6 12"
          fill="none"
      >
          <path
              d="M0.847363 11.6653C0.654963 11.6653 0.462564 11.5921 0.316164 11.4446C0.0236636 11.1516 0.0246635 10.677 0.318163 10.3841L4.18246 6.53165C4.32456 6.39005 4.40266 6.20055 4.40266 5.99945C4.40266 5.79925 4.32456 5.61075 4.18246 5.46915L0.317764 1.61565C0.0247639 1.32265 0.0238637 0.848048 0.316264 0.555148C0.608264 0.261248 1.08336 0.261248 1.37726 0.553148L5.24156 4.40665C5.66736 4.83145 5.90216 5.39685 5.90266 5.99945C5.90316 6.60205 5.66826 7.16745 5.24156 7.59415L1.37686 11.4466C1.23086 11.5921 1.03876 11.6653 0.847363 11.6653Z"
              fill="#757676"
          />
      </svg>
  ),
  nextPageTitle: "First page",
  prePageTitle: "Pre page",
  firstPageTitle: "Next page",
  lastPageTitle: "Last page",
  showTotal: true,
  paginationTotalRenderer: customTotal,
  disablePageTitle: true,
  sizePerPageList: [
      {
          text: "10",
          value: 10,
      },
      {
          text: "20",
          value: 20,
      },
  ],
};